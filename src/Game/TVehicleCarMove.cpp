/* Vehicle::TVehicleCarMove (class name from the RTTI of its bound callbacks and of its control
 * block): the driving of a car (Vehicle::TVehicleCar, Game/TVehicleCar.cpp), which owns it through
 * a shared_ptr. It keeps the car's state (its body, its two wheel pairs and the rider's pad) and
 * the state of the frame before, moves the wheels through the collision world, steers, boosts and
 * damages the car, plays its engine sounds and the rider's motions, and updates the car's model.
 * Member, struct and function names are inferred (the handling types' names "Jeep", "Open", "GUN"
 * and "BIKE" are the game's). Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * Game/TVehicleCarMove.h gives the other units the part of the class they use; this unit declares
 * the whole class (with the same layout). The unit's .data is addressed through one base and so
 * is kept whole: it holds the TNumEdit/CharaColliBaseCharInfo base lists, the handling table, the
 * pointers to members of the bound callbacks, and dead copies of weak vtables, among them
 * Sonicteam::Gindows' VariableBasic<float> (Game/GindowsVariableEditor.h).
 *
 * NonMatching. The functions that differ do so only in register allocation, scheduling or the
 * order of commutative operands, which no source shape tried changed (see docs/research-notes.md
 * and Game/VehicleColliMove.cpp):
 * - Sweep (0x801B9CA8): the register blocker of Game/VehicleColliMove.cpp's Sweep (retail gives
 *   the locals the high saved registers).
 * - UpdateModel, Move, SetPlacement, UpdateBoost, UpdateMotion: register allocation only (Move
 *   also keeps &rear in a register, two instructions more).
 * - GetForce, Run: retail loads the input level before the inner vector call and keeps it in a
 *   saved FPR; GC/2.7 loads it after the call (six instructions).
 * - GetNode, SetLamp, GetDamage, UpdateSpring: loads scheduled above stores (the AcceptHit
 *   pattern of Game/VehicleColliMove.cpp).
 * - ReadInput: the operands of the smoothing's fadds/fmuls are swapped.
 * - SetCamera: retail indexes the handling table with add/lwz where GC/2.7 uses lwzx (one
 *   load).
 * - UpdateGround: retail copies the line from the temporary fn_80020A20 constructs (the line's
 *   constructor, which other units call by that name), not from its return value.
 * .data, .bss, .sbss and .sdata2 match. .rodata differs only in the place of VariableBasic<float>'s
 * RTTI names (before, not after, the checked_deleter names); .sdata has the same order problem
 * and also holds RTTI records the linker drops. CreateTuningVariable and GetStep (at the end and
 * before AcceptHit) are unused functions that only reproduce data and a pooled constant; the
 * linker drops them. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
/* boost::function's operator() puts a bad_function_call on the stack (see <stdexcept>). */
#define MSL_RUNTIME_ERROR_STRING
#define BOOST_SP_STATIC_POINTER_CAST
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string.h>
#include "Game/TNumEdit.h"
#include "Game/MathTypes.h"

/* A vector as RenderWare takes it (RwV3d; a plain aggregate; name inferred). */
struct CarV3d {
    float x;
    float y;
    float z;
};

/* The axes and a zero vector (names inferred), and four words that no code reads. Their place in
 * .rodata, between the RTTI names of Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h, puts them
 * between those includes. */
static const CarV3d s_axisX = {1.0f, 0.0f, 0.0f};
static const CarV3d s_axisY = {0.0f, 1.0f, 0.0f};
static const CarV3d s_axisZ = {0.0f, 0.0f, 1.0f};
static const CarV3d s_zero = {0.0f, 0.0f, 0.0f};
static const float s_unused[4] = {0.0f, 0.0f, 0.0f, 0.0f};

/* A vector as RenderWare takes it (name inferred). */
inline CarV3d ToV3d(const Vec3f &v) {
    CarV3d r = s_zero;
    r.x = v.x;
    r.y = v.y;
    r.z = v.z;
    return r;
}

#include "Game/CharaColliBaseCharInfo.h"
#include "Game/AttackIgnoreList.h"
#include "Game/BirdBase.h"
#define CHARCOMMAND_EXTERNAL_MOTION_INLINE
#define CHARCOMMAND_PLAYER_COMMANDS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CollisionEffect.h"
#include "Game/DebugDisp.h"
#include "Game/GeometryLine.h"
#include "Game/GindowsVariableEditor.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/TBlockManage.h"
#include "Game/TVehicleBase.h"
#include "Game/TVehicleCamera.h"
#include "Game/TVehicleCar.h"
#include "Game/VehicleCarResource.h"
#include "Game/VehicleEffect.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using Vehicle::BirdBase::UnkBirdCollision_801B4EBC;
using Vehicle::TVehicleBase;
using Vehicle::TVehicleCamera;
using Vehicle::TVehicleCar;

namespace std {
float fabs(float x);
float sin(float x);
float cos(float x);
} // namespace std

struct RwFrame;

/* A vector copied and assigned through the out-of-line vector copy constructor and assignment
 * (fn_800091EC, fn_800091D0), as the game's vector class is (name inferred; see
 * Game/Fn8040CopyVector3.h). Its address is a Vec3f. */
struct CarVector {
    CarVector() {}
    CarVector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    explicit CarVector(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    CarVector(const CarVector &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }
    CarVector &operator=(const CarVector &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
        return *this;
    }

    float x;
    float y;
    float z;
};
SIZE_ASSERT(CarVector, 0xC);

/* Euler angles (name inferred): set to zero on construction, assigned member by member. */
struct CarAngle {
    CarAngle() : x(0.0f), y(0.0f), z(0.0f) {}

    float x;
    float y;
    float z;
};
SIZE_ASSERT(CarAngle, 0xC);

/* A matrix copied through the out-of-line matrix copy (fn_8040C148; see Game/CameraAdjust.cpp). */
struct CarMatrix {
    CarMatrix() {}
    CarMatrix(const CarMatrix &m) {
        fn_8040C148(reinterpret_cast<Mtx34f *>(this), reinterpret_cast<const Mtx34f *>(&m));
    }

    float values[3][4];
};

/* A CarVector or CarAngle as a Vec3f, and a CarMatrix as a Mtx34f. */
#define V(vector) reinterpret_cast<Vec3f &>(vector)
#define M(matrix) reinterpret_cast<Mtx34f &>(matrix)

/* The flag word of a wheel (see Game/GadgetModelInfo.h): fn_80014E04 clears it, fn_80014E10 clears
 * all bits, and its assignment is out of line (fn_8000B21C). */
extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8000B21C(u32 *flags, const u32 *source);
}

struct CarFlags {
    CarFlags() {
        fn_80014E04(&m_bits);
    }
    CarFlags &operator=(const CarFlags &flags) {
        fn_8000B21C(&m_bits, &flags.m_bits);
        return *this;
    }

    u32 m_bits;
};

/* The handling of a car type (struct name unknown; names inferred, most members keep their
 * offsets). One per handling type: "Jeep", "Open", "GUN" and "BIKE". */
struct CarParam {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 m_life;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ float field_0x18;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ float field_0x24;
    /* 0x28 */ float field_0x28;
    /* 0x2C */ float field_0x2C;
    /* 0x30 */ float field_0x30;
    /* 0x34 */ float field_0x34;
    /* 0x38 */ float field_0x38;
    /* 0x3C */ float field_0x3C;
    /* 0x40 */ float field_0x40;
    /* 0x44 */ float field_0x44;
    /* 0x48 */ float field_0x48;
    /* 0x4C */ float field_0x4C;
    /* 0x50 */ float field_0x50;
    /* 0x54 */ float field_0x54;
    /* 0x58 */ float field_0x58;
    /* 0x5C */ float field_0x5C;
    /* 0x60 */ float field_0x60;
    /* 0x64 */ float field_0x64;
    /* 0x68 */ float field_0x68;
    /* 0x6C */ float field_0x6C;
    /* 0x70 */ float field_0x70;
    /* 0x74 */ float field_0x74;
    /* 0x78 */ float field_0x78;
    /* 0x7C */ float field_0x7C;
    /* 0x80 */ const Vec3f *field_0x80;
    /* 0x84 */ const Vec3f *m_size;
    /* 0x88 */ const Vec3f *m_cameraOffset;
    /* 0x8C */ float field_0x8C;
    /* 0x90 */ float m_radius;
    /* 0x94 */ float field_0x94;
    /* 0x98 */ float field_0x98;
    /* 0x9C */ float field_0x9C;
    /* 0xA0 */ float field_0xA0;
    /* 0xA4 */ float m_slope; /* the steepest ground a wheel stands on, in radians */
    /* 0xA8 */ float field_0xA8;
};
SIZE_ASSERT(CarParam, 0xAC);

/* The surface of a hit (see Game/ContainerBehavior.cpp). */
struct UnkSurface_80021E24 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ s32 m_id;
    /* 0x18 */ u8 field_0x18[0x1C - 0x18];
    /* 0x1C */ u32 m_flags; /* the high 4 bits are the surface kind */
};

/* A hit of a collision test (see Game/ContainerBehavior.cpp). */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ UnkSurface_80021E24 *m_surface;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object;
};

/* Whether the hit surface has id -1 (an inline function of another unit, out of line). */
bool IsSpecialSurface(UnkCollision_80021E24 *hit);

/* A node of the sweep's hits (see Game/VehicleColliMove.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

struct CollisionResultList;

/* What the car's tests skip and accept (struct name unknown; a weak constructor and destructor of
 * this unit): hits on the car's two colliders and hits of attribute 0x200000 are skipped; other
 * hits count when their attributes share a bit with m_mask (names inferred). */
struct UnkCarSweepFilter_801B9ADC {
    /* 0x00 */ u32 m_mask;
    /* 0x04 */ boost::shared_ptr<CharaColliBase> m_body;
    /* 0x0C */ boost::shared_ptr<CharaColliBase> m_other;
    /* 0x14 */ TVehicleBase *m_vehicle;
};
SIZE_ASSERT(UnkCarSweepFilter_801B9ADC, 0x18);

/* What the tests pass the filter for each candidate hit (see Game/VehicleColliMove.cpp). */
struct UnkCarCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ const UnkCarSweepFilter_801B9ADC *m_filter;
};

typedef bool (*UnkCarFilter_80021934)(UnkCarCandidate_80021934 *candidate);

/* A sphere: centre and radius (see Game/VehicleColliMove.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* A pad as the car reads it, 0x28 bytes after the pads' base (struct name unknown; names inferred). */
struct UnkCarPad_80051734 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u32 m_held;
    /* 0x2C */ u8 field_0x2C[0x30 - 0x2C];
    /* 0x30 */ u32 m_pressed;
    /* 0x34 */ u8 field_0x34[0x38 - 0x34];
    /* 0x38 */ float m_stickX;
    /* 0x3C */ float m_trigger;
};

/* The pads (see Game/AdvBattleMode.cpp): the pads 0x28 bytes apart, and the pad of each player at
 * 0x11C (struct name unknown). */
struct UnkCarPads_80051734 {
    /* 0x000 */ u8 field_0x0[0x11C];
    /* 0x11C */ s32 m_ports[4];
};

/* A node of a car's model and its frame's matrix (struct names unknown). */
struct UnkCarNode_80038E84 {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ Vec3f m_position;
};

/* The game state (see Game/Enemy.h): the frame counter at 0x90, the pause flag at 0xC8. */
extern u8 lbl_8057D6A0[0x150];

/* A vector constant read by the collision setup (meaning unknown; see Game/CommandCollision.cpp). */
extern Vec3f lbl_805E07B8;

namespace Sonicteam {
namespace System {
namespace Geometry {
class Line;
} // namespace Geometry
} // namespace System
} // namespace Sonicteam

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp), and the matrix
 * routines that return a matrix. */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
typedef CarMatrix (*TranslationFn)(const Vec3f *offset);
typedef CarMatrix (*RotationFn)(float radians);
typedef CarMatrix (*MultiplyFn)(const Mtx34f *a, const Mtx34f *b);
typedef Vec3f (*MakeVectorFn)(float x, float y, float z);

extern "C" {
/* Debug output (see Game/TVehicleCar.cpp). */
void fn_803A953C(const char *format, ...);

/* The collision world and its tests (see Game/VehicleSweep.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere, const Vec3f *velocity,
                                 int arg3, int arg4, CollisionResultList *previous,
                                 UnkCarFilter_80021934 filter, const UnkCarSweepFilter_801B9ADC *self,
                                 int arg8);
UnkCollision_80021E24 *fn_80021E24(void *world, const UnkDebugLine_80020A20 *line, int arg2, int arg3,
                                   UnkCarFilter_80021934 filter, const UnkCarSweepFilter_801B9ADC *self);
void fn_800265A4(UnkCollision_80021E24 *hit, int arg1);
UnkDebugLine_80020A20 *fn_80020A20(void *line, const Vec3f *start, const Vec3f *end);
void fn_80026FB4(CollisionResultList *hits, int arg1);
UnkColliContactNode_800587A0 *fn_8001EE04(CollisionResultList *hits);
UnkColliContactNode_800587A0 *fn_8001EDEC(CollisionResultList *hits);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
int fn_80099080(void *object);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);

/* The vehicle's colliders and life (see Game/TVehicleBase.cpp). */
boost::shared_ptr<CharaColliBase> fn_801B6350(TVehicleBase *vehicle);
boost::shared_ptr<CharaColliBase> fn_801B6380(TVehicleBase *vehicle);
void fn_801B6470(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider);
void fn_801B63D4(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider);
void fn_801B6030(TVehicleBase *vehicle, int arg1);
float fn_801B64F8(TVehicleBase *vehicle);
void fn_801B6510(TVehicleBase *vehicle, float delta);
void fn_801B6570(TVehicleBase *vehicle, float value);
void fn_801B5FA4(TVehicleBase *vehicle);
void fn_801B6944(TVehicleBase *vehicle);
void fn_801B6860(TVehicleBase *vehicle, CharCommand *command);
void fn_801C067C(TVehicleBase *vehicle, const boost::shared_ptr<CharaColliBase> &collider);
/* Whether a line meets the player's ground of attribute bit 5 (Game/VehicleSweep.cpp). */
bool fn_801BFB70(const Sonicteam::System::Geometry::Line *line);

/* The bird's collision (see Game/BirdBase.cpp). */
void fn_801B4DD0(UnkBirdCollision_801B4EBC *collision, TVehicleBase *vehicle);
void fn_801B4DAC(UnkBirdCollision_801B4EBC *collision, const boost::shared_ptr<CharaColliBase> *collider);
void fn_801B4D18(UnkBirdCollision_801B4EBC *collision, const AttackCallback &callback);
void fn_801B4D3C(UnkBirdCollision_801B4EBC *collision, const AttackCallback &onHit, const AttackCallback &onEnd);
void fn_801B4CF4(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D60(UnkBirdCollision_801B4EBC *collision);

/* The collider manager and a box collider (see Game/CommandCollision.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, boost::weak_ptr<CharInfo> charInfo, const Vec3f *size,
                                              int flags, int arg4, int arg5, const Vec3f *offset,
                                              const Vec3f *rotation, int arg8);

/* The list of attacks that hit the car (see Game/PlayerBarrierHeatEffect.cpp). */
void fn_803555CC(void *list);
void fn_80355478(void *list, int flag);
bool fn_803553E8(void *list, const void *target);
void fn_803552DC(void *list, const void *target);
void fn_80355260(void *list, int count);
void fn_8035511C(void *list);

/* The pads (see Game/AdvBattleMode.cpp). */
UnkCarPads_80051734 *fn_80051734(void);

/* Sounds (see Game/GadgetUtil.h and Game/AudioLoopManagerTask.cpp). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
void fn_801D53C4(void *manager, int id, void *source, void *owner, int arg4, s8 arg5, int arg6);

/* Vector helpers (see Game/VehicleSweep.cpp). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CE60(Vec3f *vector);
void fn_8040CE74(Vec3f *vector);
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
Vec3f fn_80093230(const Vec3f *direction);
Vec3f fn_80016284(const Vec3f *vector);
/* A matrix times a vector, with and without its translation (behaviour inferred). */
Vec3f fn_8040B618(const Mtx34f *m, const Vec3f *v);
Vec3f fn_8040B67C(const Mtx34f *m, const Vec3f *v);
/* Rotation matrices about Z, Y and X (behaviour inferred). */
void fn_8040B0C0(Mtx34f *m, float radians);
void fn_8040B138(Mtx34f *m, float radians);
void fn_8040B1B0(Mtx34f *m, float radians);
/* Moves position by velocity, pulling velocity towards target at rate (see Game/BirdBase.cpp). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *target, float rate, float dt);
/* The angles of a direction, and their inverse (behaviour inferred). */
void fn_8003B9E8(const Vec3f *direction, float *x, float *y);
void fn_8003B8F4(const Vec3f *direction, float *x, float *y);
/* An angle moved towards another (behaviour inferred). */
float fn_8004924C(float angle, float target, float rate, float limit);
/* Euler angles of a matrix (behaviour inferred). */
void fn_800493C4(Vec3f *angles, const Mtx34f *m);
void fn_800494F8(Vec3f *angles, const Mtx34f *m);
/* Matrix product (behaviour inferred). */
void fn_8001E044(Mtx34f *out, const Mtx34f *a, const Mtx34f *b);
/* A 3x3 matrix of a frame (behaviour inferred). */
void fn_800245C8(CarV3d *a, CarV3d *b, int arg, float radius);

/* The car's clumps and frames (behaviour inferred). */
void *fn_80038E84(void *clump, int node);
void fn_80038F2C(void *part, void *frame, int arg);
void fn_80038F60(void *part, void *frame, int arg);
void fn_80038F94(void *part);
void fn_80038FC0(void *part);
RwFrame *fn_8047F994(void);
void fn_8047F59C(RwFrame *frame, const CarV3d *v, int combine);
void fn_8047F958(RwFrame *frame);
void fn_8047ECD4(RwFrame *frame, const CarV3d *axis, float oneMinusCosine, float sine, int combine);
void fn_804876E8(void *frame, const void *matrix, int arg);
void fn_80487784(void *frame, const CarV3d *axis, float angle, int combine);
void fn_804875B0(void *frame, const CarV3d *v, int combine);
void *RwFrameGetLTM(void *frame);
void fn_80415D40(void *motion, int arg, float *out);
void fn_804173A8(void *motion, const Vec3f *position, const Vec3f *rotation, int arg);
void fn_80416E20(void *motion, int arg, float time);
void fn_80415EA4(void *motion);
void fn_800159AC(void *updater);
}

/* An object of 0x18 bytes at the start of the unit's .bss that no code of the unit reads (type and
 * name unknown). */
struct UnkCarObject_80581068 {
    /* 0x00 */ u8 field_0x0[0x18];
};
UnkCarObject_80581068 lbl_80581068;

/* The handling of the car types (names inferred). */
static CarVector s_jeepAnchor(0.0f, 16.0f, 0.0f);
static CarVector s_jeepSize(6.0f, 5.0f, 15.0f);
static CarVector s_jeepCamera(0.0f, 2.0f, 65.0f);
static CarVector s_openAnchor(0.0f, 16.0f, 0.0f);
static CarVector s_openSize(6.0f, 5.0f, 15.0f);
static CarVector s_openCamera(0.0f, 2.0f, 60.0f);
static CarVector s_gunAnchor(0.0f, 18.0f, 0.0f);
static CarVector s_gunSize(8.0f, 8.0f, 22.0f);
static CarVector s_gunCamera(0.0f, 10.0f, 75.0f);
static CarVector s_bikeAnchor(0.0f, 16.0f, 0.0f);
static CarVector s_bikeSize(3.0f, 5.0f, 15.0f);
static CarVector s_bikeCamera(0.0f, 2.0f, 45.0f);

/* Degrees to radians (name inferred): pi is an extern constant, so these are stored by the static
 * initializer. */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

static CarParam s_params[4] = {
    {"Jeep", 6, 70, 3.0f, 24.0f, -0.1f, 450.0f, 180.0f, 360.0f, 180.0f, 1.0f, 1.5f, 9.0f, -127.0f, -9.0f,
     -0.1f, 3.0f, 14.0f, 0.97f, 0.011f, 2.6f, 1.0f, 1.52f, 360.0f, 1.3f, 150.0f, 0.2f, 60.0f, 1.0f, 0.03f,
     0.075f, 1.75f, &V(s_jeepAnchor), &V(s_jeepSize), &V(s_jeepCamera), 13.0f, 12.0f, 9.5f, -1.5f, -0.0003f, 0.1f,
     ToRadians(45.0f), 0.007f},
    {"Open", 6, 50, 2.4f, 19.2f, -0.1f, 600.0f, 180.0f, 450.0f, 225.0f, 1.0f, 1.5f, 9.0f, -127.0f, -9.0f,
     -0.1f, 3.0f, 14.0f, 0.97f, 0.011f, 2.6f, 1.0f, 1.3f, 360.0f, 1.3f, 180.0f, 0.2f, 60.0f, 1.0f, 0.03f,
     0.075f, 1.75f, &V(s_openAnchor), &V(s_openSize), &V(s_openCamera), 13.0f, 12.0f, 9.5f, -1.5f, -0.0003f, 0.1f,
     ToRadians(45.0f), 0.007f},
    {"GUN", 10, 200, 3.0f, 24.0f, -0.1f, 450.0f, 135.0f, 230.0f, 115.0f, 1.0f, 1.5f, 9.0f, -127.0f, -9.0f,
     -0.1f, 3.0f, 14.0f, 0.97f, 0.011f, 2.6f, 1.0f, 1.52f, 360.0f, 1.3f, 200.0f, 0.2f, 60.0f, 1.0f, 0.03f,
     0.075f, 1.75f, &V(s_gunAnchor), &V(s_gunSize), &V(s_gunCamera), 20.0f, 12.0f, 10.5f, -1.5f, -0.0003f, 0.1f,
     ToRadians(45.0f), 0.007f},
    {"BIKE", 4, 40, 1.5f, 14.0f, -0.1f, 700.0f, 150.0f, 350.0f, 200.0f, 1.0f, 1.3f, 9.0f, -127.0f, -9.0f,
     -0.1f, 3.0f, 14.0f, 0.97f, 0.011f, 2.6f, 1.0f, 1.52f, 12.0f, 0.3f, 150.0f, 0.2f, 60.0f, 0.5f, 0.04f,
     0.1f, 1.75f, &V(s_bikeAnchor), &V(s_bikeSize), &V(s_bikeCamera), 13.0f, 8.0f, 5.5f, 7.0f, 0.0f, 0.6f,
     ToRadians(60.0f), 0.01f},
};

/* Tuning values (names inferred). */
static float s_bounceScale = 0.0066666668f;
static float s_bounceLimit = 1.5f;
static float s_springDamp = 0.8f;
static float s_springPull = 0.1f;
static float s_bounceTime = 0.25f;
static float s_slowSpeed = 25.0f;
static float s_stuckTime = 1.5f;
static float s_radiusMargin = 0.1f;
static float s_pushScale = 0.5f;
static float s_steerRate = 8.0f;
static float s_accelRiseRate = 18.0f;
static float s_accelFallRate = 6.0f;
static float s_boostRiseRate = 18.0f;
static float s_boostFallRate = 6.0f;
static float s_brakeRiseRate = 18.0f;
static float s_brakeFallRate = 6.0f;

/* A length (see Game/VehicleSweep.cpp): as a call it is not a constant expression, so the constant
 * below is stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* How far the sweep pushes away from what it touched (name inferred). */
static float s_skin = Span(0.020000001f);


/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<TVehicleCamera>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* vector = value through the out-of-line assignment, and the out-of-line copy constructor. */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}
inline void CopyVector(Vec3f &vector, const Vec3f &value) {
    fn_800091EC(&vector, &value);
}

namespace Vehicle {

class TVehicleCarMove {
  public:
    /* The rider's pad as the car reads it (struct name unknown). */
    struct Input {
        /* 0x00 */ float m_steer;
        /* 0x04 */ bool m_horn;
        /* 0x05 */ bool m_getOff;
        /* 0x06 */ bool m_accel;
        /* 0x07 */ bool m_brake;
        /* 0x08 */ bool m_boost;
        /* 0x0C */ float m_accelLevel;
        /* 0x10 */ float m_brakeLevel;
        /* 0x14 */ float m_boostLevel;
    };

    /* The boost (struct name unknown). */
    struct Boost {
        /* 0x00 */ float m_time;
        /* 0x04 */ float m_charge;
        /* 0x08 */ float m_hold;
        /* 0x0C */ float m_gauge;
        /* 0x10 */ u8 field_0x10;
        /* 0x14 */ CarVector m_push;
    };

    /* The model's placement (struct name unknown). */
    struct Model {
        /* 0x00 */ float m_spin[4];
        /* 0x10 */ CarVector m_position;
        /* 0x1C */ CarAngle m_rotation;
    };

    /* A pair of wheels (struct name unknown). */
    struct Wheel {
        /* 0x00 */ CarVector m_offset;
        /* 0x0C */ CarVector m_velocity;
        /* 0x18 */ CarVector m_normal;
        /* 0x24 */ CarFlags m_flags; /* bit 0 on the ground */
        /* 0x28 */ float m_spring;
        /* 0x2C */ float m_springOld;
        /* 0x30 */ float m_air;
    };

    /* The car's state (struct name unknown). */
    struct State {
        /* 0x000 */ s32 m_mode; /* 1 parked, 2 ridden */
        /* 0x004 */ CarVector m_position;
        /* 0x010 */ CarAngle m_rotation;
        /* 0x01C */ CarVector m_up;
        /* 0x028 */ CarVector m_velocity;
        /* 0x034 */ float m_speed;
        /* 0x038 */ s32 m_row;
        /* 0x03C */ float m_time;
        /* 0x040 */ s32 m_wait;
        /* 0x044 */ float m_pitch;
        /* 0x048 */ float m_roll;
        /* 0x04C */ float m_lift;
        /* 0x050 */ float m_stuck;
        /* 0x054 */ Input m_input;
        /* 0x06C */ Boost m_boost;
        /* 0x08C */ Model m_model;
        /* 0x0B4 */ Wheel m_wheels[2];
    };

    /* The car's position and rotation. */
    struct Body {
        /* 0x0 */ CarVector m_position;
        /* 0x0C */ CarAngle m_rotation;
    };

    /* The car's motion (struct name unknown). */
    struct Drive {
        /* 0x00 */ float m_speed;
        /* 0x04 */ float m_steer;
        /* 0x08 */ CarVector m_velocity;
        /* 0x14 */ u8 m_bounced;
        /* 0x18 */ s32 m_damage;
        /* 0x1C */ u8 m_hit;
    };

    /* The camera's target and the ground (struct name unknown). */
    struct Target {
        /* 0x00 */ CarVector m_position;
        /* 0x0C */ u8 m_lampOff;
        /* 0x0D */ u8 m_noBrake;
        /* 0x0E */ u8 m_grounded;
        /* 0x10 */ CarVector m_ground;
    };

    TVehicleCarMove(TVehicleCar *car, int type);
    ~TVehicleCarMove();

    static int ToType(int kind);
    static void Initialize(int kind);

    void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    void OnCommand(CharCommand *command);
    void UpdateModel();
    void Tick(float dt);
    void Start();
    void SetCamera(int mode, int arg);

    void ReleaseAttack();
    void OnDamaged(AttackCallbackParam &param);
    void OnAttackHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    void UpdateSound();
    void ReadInput(float dt);
    void UpdateBoost(float dt);
    CarVector GetForce(Wheel &wheel, const Mtx34f *matrix);
    void SlideWheel(float dt, Wheel &wheel, const Vec3f &force, const Vec3f &right);
    void TurnWheel(float dt, Wheel &wheel, const Vec3f &force, const Vec3f &right);
    float GetSteerAngle();
    void Run(float dt);
    void UpdateMotion(float dt);
    void CheckFall();
    void CreateEffects();
    void UpdatePart(void *part);
    void MoveWheel(float dt, Wheel &wheel, Vec3f *position);
    void PushWheel(UnkSphere_80021C04 &a, CarVector &delta, UnkSphere_80021C04 &b);
    void Move(float dt);
    void UpdateSpring();
    int GetDamage();
    void UpdateStuck(float dt);
    void BeginFrame();
    void UpdateGround();
    void CreateColliders();
    void SetBodyCollider(int flags, int arg, float sx, float sy, float sz);
    void UpdateWheelOffsets();
    void *GetNode(int index);
    void SetLamp(bool on);
    void GetOff();
    bool SweepWheel(Vec3f *ground, Vec3f *normal, TVehicleBase *vehicle, Vec3f *position, Vec3f *velocity,
                    u32 *flags, const Vec3f *up, float radius);
    bool IsOnGround(State &state, Wheel &wheel);
    float GetStep();
    bool LineTest(UnkDebugLine_80020A20 line, Vec3f *position, u32 *kind);

    Body *GetBody() {
        return &m_body;
    }
    const CarModelDesc &ModelDesc() {
        return lbl_804CDAF8[m_type];
    }
    const CarParam &Param() {
        return s_params[m_type];
    }
    UnkCarPad_80051734 &Pad(int index) {
        return *reinterpret_cast<UnkCarPad_80051734 *>(fn_80051734()->field_0x0 + index * 0x28);
    }
    /* Smooths value towards target (name inferred). */
    static void Smooth(float &value, float target, float rate) {
        value = target + ((value - target) * rate) / (rate + 1.0f);
    }
    /* Fills and drains the boost gauge (names inferred). */
    void ChargeGauge(float dt) {
        const CarParam &param = s_params[m_type];
        m_states[0].m_boost.m_gauge += param.field_0x78 * dt;
        if (m_states[0].m_boost.m_gauge >= param.field_0x74) {
            m_states[0].m_boost.m_gauge = param.field_0x74;
        }
    }
    void DrainGauge(float dt) {
        const CarParam &param = s_params[m_type];
        m_states[0].m_boost.m_gauge -= param.field_0x78 * dt;
        if (m_states[0].m_boost.m_gauge < 0.0f) {
            m_states[0].m_boost.m_gauge = 0.0f;
        }
    }
    void SetMotion(int motion) {
        if (m_states[0].m_mode == 2 && m_motion != 5) {
            m_motion = motion;
        }
    }

    /* 0x000 */ s32 m_frame;
    /* 0x004 */ State m_states[2]; /* now and the frame before */
    /* 0x23C */ Drive m_drive;
    /* 0x25C */ s32 m_type;
    /* 0x260 */ s32 m_pad;
    /* 0x264 */ TVehicleCar *m_car;
    /* 0x268 */ boost::shared_ptr<TVehicleCamera> m_camera;
    /* 0x270 */ boost::shared_ptr<CharaColliBase> m_body2;
    /* 0x278 */ boost::shared_ptr<CharaColliBase> m_bodyCollider;
    /* 0x280 */ UnkBirdCollision_801B4EBC *m_attack;
    /* 0x284 */ Body m_body;
    /* 0x29C */ s32 m_motion;
    /* 0x2A0 */ s32 m_lastMotion;
    /* 0x2A4 */ s32 m_motionWait;
    /* 0x2A8 */ Target m_target;
    /* 0x2C4 */ boost::shared_ptr<CollisionEffect> m_effects[4];
    /* 0x2E4 */ u8 m_hits[0xC];
};
SIZE_ASSERT(TVehicleCarMove, 0x2F0);

} // namespace Vehicle

using Vehicle::TVehicleCarMove;

/* The command that plays a car's rider motions (inline in the game; see Game/CharCommand.h). */
inline Player::SetExternalMotionCommand::SetExternalMotionCommand(const s32 *motions, int arg1, float time)
    : PlayerCommand(0x12A, arg1) {
    *reinterpret_cast<UnkCarFiles *>(m_motions) = *reinterpret_cast<const UnkCarFiles *>(motions);
    m_time = time;
}

/* The view of AttackCallbackParam the car's callbacks read (names inferred). */
struct CarAttackParam {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ boost::weak_ptr<CharaColliAttack> m_target;
    /* 0x10 */ u8 field_0x10[0x30 - 0x10];
    /* 0x30 */ u8 m_ignored;
    /* 0x31 */ u8 field_0x31[0x50 - 0x31];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define CarParam_(param) (*(CarAttackParam *)&(param))

void TVehicleCarMove::Initialize(int kind) {}

TVehicleCarMove::TVehicleCarMove(TVehicleCar *car, int type) : m_type(type), m_car(car) {
    fn_803555CC(m_hits);
    memset(m_states, 0, sizeof(m_states));
    memset(&m_drive, 0, sizeof(m_drive));
    m_motionWait = 0;
    m_lastMotion = 0;
}

TVehicleCarMove::~TVehicleCarMove() {
    ReleaseAttack();
    fn_80355478(m_hits, -1);
}

void TVehicleCarMove::Start() {
    SetCamera(1, 0);
    CreateColliders();
    CreateEffects();
    SetPlacement(0, 0);
}

void TVehicleCarMove::ReleaseAttack() {
    fn_801B4CF4(m_attack);
    fn_801B4D60(m_attack);
    delete m_attack;
}

void TVehicleCarMove::SetCamera(int mode, int arg) {
    if (mode == m_states[0].m_mode) {
        return;
    }
    switch (mode) {
    case 1:
        m_camera.reset();
        break;
    case 2:
        UnkCarPads_80051734 *pads = fn_80051734();
        m_pad = pads->m_ports[arg];
        m_camera = boost::shared_ptr<TVehicleCamera>(new TVehicleCamera(
            &V(m_target.m_position), &V(m_states[0].m_rotation), Param().m_cameraOffset));
        SetMotion(6);
        SetBodyCollider(0x200, 2, 1.25f, 1.25f, Param().field_0x7C);
        fn_801B4DAC(m_attack, &m_bodyCollider);
        fn_801B4D18(m_attack, AttackCallback(boost::bind(&TVehicleCarMove::OnDamaged, this, _1)));
        fn_801B4D3C(m_attack, AttackCallback(boost::bind(&TVehicleCarMove::OnAttackHit, this, _1)),
                    AttackCallback(boost::bind(&TVehicleCarMove::OnAttackEnd, this, _1)));
        m_target.m_noBrake = 1;
        break;
    case 3:
        m_camera.reset();
        break;
    }
    m_states[0].m_mode = mode;
}

void TVehicleCarMove::Tick(float dt) {
    if (lbl_8057D6A0[0xC8] != 0) {
        return;
    }
    if (m_states[0].m_mode == 1 && !m_target.m_grounded && m_states[0].m_wheels[1].m_air >= 0.1f) {
        boost::shared_ptr<TBlockManage::forBlockCheck> block = fn_8003A154(&V(m_target.m_ground));
        if (fn_80039EDC(boost::weak_ptr<TBlockManage::forBlockCheck>(block))) {
            return;
        }
    }
    BeginFrame();
    switch (m_states[0].m_mode) {
    case 1:
        memset(&m_states[0].m_input, 0, sizeof(Input));
        Run(dt);
        Move(dt);
        UpdateStuck(dt);
        UpdateMotion(dt);
        UpdateSound();
        break;
    case 2:
        ReadInput(dt);
        Run(dt);
        Move(dt);
        UpdateStuck(dt);
        fn_801C067C(m_car, fn_801B6380(m_car));
        UpdateMotion(dt);
        UpdateSound();
        break;
    }
    UpdateGround();
    if (m_states[0].m_mode == 2) {
        CheckFall();
    }
    if (fn_801B64F8(m_car) <= 0.0f) {
        fn_801D560C(fn_80053164(), 0x202E, 0, 0);
        if (m_car->field_0x74 != 0) {
            CarMatrix matrix = ((TranslationFn)fn_8040B034)(&V(m_states[0].m_position));
            fn_8040B80C(&M(matrix), m_states[0].m_rotation.z);
            fn_8040B884(&M(matrix), m_states[0].m_rotation.x);
            fn_8040B848(&M(matrix), m_states[0].m_rotation.y);
            reinterpret_cast<Vehicle::Effect *>(m_car->field_0x74)->StartBurst(&M(matrix));
        }
        fn_801B6030(m_car, 0);
        fn_801B6944(m_car);
    }
    m_frame++;
}

/* Rotations of a matrix about Z, X and Y (names inferred). */
static void RotateZ(RwFrame *matrix, float angle, int combine) {
    fn_8047ECD4(matrix, &s_axisZ, 1.0f - std::cos(angle), std::sin(angle), combine);
}

static void RotateX(RwFrame *matrix, float angle, int combine) {
    fn_8047ECD4(matrix, &s_axisX, 1.0f - std::cos(angle), std::sin(angle), combine);
}

static void RotateY(RwFrame *matrix, float angle, int combine) {
    fn_8047ECD4(matrix, &s_axisY, 1.0f - std::cos(angle), std::sin(angle), combine);
}

/* Degrees from radians (name inferred). */
inline float ToDegrees(float radians) {
    return radians * (180.0f / lbl_805FAD38);
}

void TVehicleCarMove::UpdateModel() {
    State &state = m_states[0];
    RwFrame *body = fn_8047F994();
    CarV3d position = ToV3d(V(state.m_position));
    fn_8047F59C(body, &position, 0);
    RotateZ(body, state.m_rotation.z, 1);
    RotateX(body, state.m_rotation.x, 1);
    RotateY(body, state.m_rotation.y, 1);
    if (m_type == 3) {
        RotateZ(body, state.m_roll, 1);
    }
    RwFrame *model = fn_8047F994();
    CarV3d modelPosition = ToV3d(V(state.m_model.m_position));
    CarV3d lift = {0.0f, state.m_lift, 0.0f};
    RotateX(model, -state.m_boost.m_gauge, 0);
    RotateZ(model, state.m_roll, 2);
    RotateX(model, state.m_pitch, 2);
    fn_8047F59C(model, &lift, 2);
    RotateY(model, state.m_model.m_rotation.y, 2);
    RotateX(model, state.m_model.m_rotation.x, 2);
    RotateZ(model, state.m_model.m_rotation.z, 2);
    fn_8047F59C(model, &modelPosition, 2);
    int i;
    for (i = 0; i < 3; i++) {
        if (m_car->m_parts[i].m_clump != 0) {
            UpdatePart(m_car->m_parts[i].m_clump);
            void *frame = *reinterpret_cast<void **>(reinterpret_cast<u8 *>(m_car->m_parts[i].m_clump) + 4);
            if (i == 1) {
                fn_804876E8(frame, body, 0);
            } else if (i == 2 && m_type == 3) {
                fn_804876E8(frame, RwFrameGetLTM(GetNode(11)), 0);
            } else {
                fn_804876E8(frame, model, 0);
            }
        }
    }
    if (state.m_row != m_states[1].m_row) {
        m_car->DestroyRow(m_states[1].m_row);
        m_car->CreateRow(state.m_row);
        SetLamp(state.m_input.m_boost);
        if (m_type == 3) {
            fn_801D560C(fn_80053164(), 0x202E, 0, 0);
        } else {
            fn_801D560C(fn_80053164(), 0x203C, 0, 0);
        }
    }
    for (i = 0; i < 3; i++) {
        int row = state.m_row;
        if (m_car->m_rows[row][i].m_clump != 0) {
            UpdatePart(m_car->m_rows[row][i].m_clump);
            if (m_car->m_rows[row][i].m_motion != 0) {
                Vec3f position = V(state.m_model.m_position);
                Vec3f rotation;
                Vec3f angles(0.0f, 0.0f, 0.0f);
                CarMatrix matrix = ((RotationFn)fn_8040B0C0)(state.m_model.m_rotation.z);
                fn_8040B884(&M(matrix), state.m_model.m_rotation.x);
                fn_8040B848(&M(matrix), state.m_model.m_rotation.y);
                fn_8040B59C(&M(matrix), &Vec3f(0.0f, state.m_lift, 0.0f));
                fn_8040B884(&M(matrix), state.m_pitch);
                fn_8040B80C(&M(matrix), state.m_roll);
                fn_8040B884(&M(matrix), -state.m_boost.m_gauge);
                fn_800494F8(&angles, &M(matrix));
                rotation = angles;
                fn_804173A8(m_car->m_rows[row][i].m_motion, &position, &rotation, 1);
                fn_80416E20(m_car->m_rows[row][i].m_motion, 0, state.m_time);
                fn_80415EA4(m_car->m_rows[row][i].m_motion);
                fn_800159AC(m_car->m_rows[row][i].m_updater);
            } else {
                fn_804876E8(*reinterpret_cast<void **>(reinterpret_cast<u8 *>(m_car->m_rows[state.m_row][i].m_clump) + 4),
                            model, 0);
            }
        }
    }
    if (state.m_input.m_boost != m_states[1].m_input.m_boost) {
        SetLamp(state.m_input.m_boost);
    }
    for (i = 0; i < 4; i++) {
        void *node;
        switch (i) {
        case 0:
            node = GetNode(1);
            if (node != 0) {
                CarV3d nodePosition = *reinterpret_cast<CarV3d *>(&reinterpret_cast<UnkCarNode_80038E84 *>(node)->m_position);
                fn_80487784(node, &s_axisX, ToDegrees(state.m_model.m_spin[i]), 0);
                fn_80487784(node, &s_axisY, ToDegrees(m_drive.m_steer), 2);
                fn_804875B0(node, &nodePosition, 2);
            }
            break;
        case 1:
            node = GetNode(0);
            if (node != 0) {
                CarV3d nodePosition = *reinterpret_cast<CarV3d *>(&reinterpret_cast<UnkCarNode_80038E84 *>(node)->m_position);
                fn_80487784(node, &s_axisX, ToDegrees(state.m_model.m_spin[i]), 0);
                fn_80487784(node, &s_axisY, ToDegrees(m_drive.m_steer), 2);
                fn_804875B0(node, &nodePosition, 2);
            }
            break;
        case 2:
            node = GetNode(3);
            if (node != 0) {
                CarV3d nodePosition = *reinterpret_cast<CarV3d *>(&reinterpret_cast<UnkCarNode_80038E84 *>(node)->m_position);
                fn_80487784(node, &s_axisX, ToDegrees(state.m_model.m_spin[i]), 0);
                fn_804875B0(node, &nodePosition, 2);
            }
            break;
        case 3:
            node = GetNode(2);
            if (node != 0) {
                CarV3d nodePosition = *reinterpret_cast<CarV3d *>(&reinterpret_cast<UnkCarNode_80038E84 *>(node)->m_position);
                fn_80487784(node, &s_axisX, ToDegrees(state.m_model.m_spin[i]), 0);
                fn_804875B0(node, &nodePosition, 2);
            }
            break;
        }
    }
    fn_8047F958(body);
    fn_8047F958(model);
    void *seat = GetNode(8);
    if (seat != 0) {
        Mtx34f *ltm = reinterpret_cast<Mtx34f *>(RwFrameGetLTM(seat));
        m_body.m_position.x = ltm->values[3][0];
        m_body.m_position.y = ltm->values[3][1];
        m_body.m_position.z = ltm->values[3][2];
    }
    CarMatrix matrix = ((RotationFn)fn_8040B0C0)(state.m_model.m_rotation.z);
    fn_8040B884(&M(matrix), state.m_model.m_rotation.x);
    fn_8040B848(&M(matrix), state.m_model.m_rotation.y);
    fn_8040B884(&M(matrix), state.m_pitch);
    fn_8040B80C(&M(matrix), state.m_roll);
    fn_8040B884(&M(matrix), -state.m_boost.m_gauge);
    fn_800494F8(&V(m_body.m_rotation), &M(matrix));
    const CarParam &param = Param();
    m_target.m_position.x = state.m_position.x;
    m_target.m_position.z = state.m_position.z;
    m_target.m_position.y = state.m_position.y + param.field_0x8C;
}

void TVehicleCarMove::OnCommand(CharCommand *command) {
    switch (command->TypeId()) {
    case 3: {
        SetAngleCommand *angle = dynamic_cast<SetAngleCommand *>(command);
        if (angle != 0) {
            m_states[0].m_rotation.y = angle->m_angles.y;
        }
        command->Flags() |= 2;
        break;
    }
    case 4:
        m_states[0].m_speed = 0.0f;
        fn_800091D0(&V(m_states[0].m_velocity), &Vec3f(0.0f, 0.0f, 0.0f));
        fn_800091D0(&V(m_states[0].m_wheels[0].m_velocity), &Vec3f(0.0f, 0.0f, 0.0f));
        fn_800091D0(&V(m_states[0].m_wheels[1].m_velocity), &Vec3f(0.0f, 0.0f, 0.0f));
        command->Flags() |= 2;
        break;
    case 5: {
        SetVelocityCommand *velocity = dynamic_cast<SetVelocityCommand *>(command);
        if (velocity != 0) {
            fn_800091D0(&V(m_states[0].m_velocity), &(const Vec3f &)fn_8040CDA8(&velocity->m_velocity, s_pushScale));
            m_states[0].m_speed = fn_8040CF50(&V(m_states[0].m_velocity));
            fn_800091D0(&V(m_states[0].m_wheels[0].m_velocity), &(const Vec3f &)fn_8040CDA8(&velocity->m_velocity, s_pushScale));
            fn_800091D0(&V(m_states[0].m_wheels[1].m_velocity), &(const Vec3f &)fn_8040CDA8(&velocity->m_velocity, s_pushScale));
            CarMatrix matrix = ((RotationFn)fn_8040B1B0)(-m_states[0].m_rotation.x);
            fn_8040B80C(&M(matrix), -m_states[0].m_rotation.z);
            Vec3f direction;
            fn_800091EC(&direction, &(const Vec3f &)fn_8040B618(&M(matrix), &velocity->m_velocity));
            Vec3f angles(0.0f, 0.0f, 0.0f);
            fn_8003B9E8(&(const Vec3f &)fn_80016284(&direction), &angles.x, &angles.y);
            m_states[0].m_rotation.y = angles.y;
            UpdateWheelOffsets();
        }
        command->Flags() |= 2;
        break;
    }
    case 7:
        break;
    }
    if (!(command->Flags() & 2)) {
        fn_801B6860(m_car, command);
    }
}

void TVehicleCarMove::UpdateSound() {
    if (m_states[0].m_mode == 2 && m_states[0].m_input.m_boost != 0) {
        fn_801D53C4(fn_80053164(), 0x202C, 0, reinterpret_cast<u8 *>(this) + 0x202C, 2, 0, 5);
    }
    if (m_states[0].m_boost.m_time && m_states[0].m_mode == 2) {
        int volume = (int)(47.0f + 80.0f * (m_states[0].m_boost.m_time / 60.0f));
        if (volume > 127) {
            volume = 127;
        }
        fn_801D53C4(fn_80053164(), 0x203B, 0, reinterpret_cast<u8 *>(this) + 0x203B, 2, volume, 3);
    }
    if (m_drive.m_bounced) {
        fn_801D560C(fn_80053164(), 0x2030, 0, 0);
        m_drive.m_bounced = 0;
    }
    if (m_states[0].m_mode == 2) {
        if (m_target.m_noBrake) {
            int volume = (int)(-127.0f * (std::fabs(m_states[0].m_speed) / 2.0f));
            if (volume > 0) {
                volume = 0;
            }
            if (volume < -127) {
                volume = -127;
            }
            switch (m_type) {
            case 0:
                fn_801D53C4(fn_80053164(), 0x2034, 0, reinterpret_cast<u8 *>(this) + 0x2034, 2, volume, 3);
                break;
            case 1:
                fn_801D53C4(fn_80053164(), 0x2031, 0, reinterpret_cast<u8 *>(this) + 0x2031, 2, volume, 3);
                break;
            case 2:
                fn_801D53C4(fn_80053164(), 0x2037, 0, reinterpret_cast<u8 *>(this) + 0x2037, 2, volume, 3);
                break;
            case 3:
                fn_801D53C4(fn_80053164(), 0x203F, 0, reinterpret_cast<u8 *>(this) + 0x203F, 2, volume, 3);
                break;
            }
        }
        if (!m_target.m_noBrake) {
            int volume = (int)(-127.0f + 127.0f * (std::fabs(m_states[0].m_speed) / 2.0f));
            if (volume > 0) {
                volume = 0;
            }
            if (volume < -127) {
                volume = -127;
            }
            switch (m_type) {
            case 0:
                fn_801D53C4(fn_80053164(), 0x2036, 0, reinterpret_cast<u8 *>(this) + 0x2036, 2, volume, 3);
                break;
            case 1:
                fn_801D53C4(fn_80053164(), 0x2033, 0, reinterpret_cast<u8 *>(this) + 0x2033, 2, volume, 3);
                break;
            case 2:
                fn_801D53C4(fn_80053164(), 0x2039, 0, reinterpret_cast<u8 *>(this) + 0x2039, 2, volume, 3);
                break;
            case 3:
                fn_801D53C4(fn_80053164(), 0x2041, 0, reinterpret_cast<u8 *>(this) + 0x2041, 2, volume, 3);
                break;
            }
        }
    }
}

void TVehicleCarMove::ReadInput(float dt) {
    Input &input = m_states[0].m_input;
    float stick = Pad(m_pad).m_stickX;
    Smooth(input.m_steer, stick, s_steerRate);
    if ((Pad(m_pad).m_held & 1) == 1) {
        input.m_accel = 1;
        Smooth(input.m_accelLevel, 1.0f, s_accelRiseRate);
    } else {
        input.m_accel = 0;
        Smooth(input.m_accelLevel, 0.0f, s_accelFallRate);
    }
    if ((Pad(m_pad).m_pressed & 1) == 1) {
        input.m_horn = 1;
        if (m_target.m_noBrake) {
            m_target.m_noBrake = 0;
        }
    } else {
        input.m_horn = 0;
    }
    if ((Pad(m_pad).m_held & 8) == 8) {
        input.m_boost = 1;
        Smooth(input.m_boostLevel, 1.0f, s_boostRiseRate);
    } else {
        input.m_boost = 0;
        Smooth(input.m_boostLevel, 0.0f, s_boostFallRate);
    }
    if (Pad(m_pad).m_trigger > 0.45f) {
        input.m_brake = 1;
        Smooth(input.m_brakeLevel, 1.0f, s_brakeRiseRate);
        if (m_target.m_noBrake) {
            m_target.m_noBrake = 0;
        }
    } else {
        input.m_brake = 0;
        Smooth(input.m_brakeLevel, 0.0f, s_brakeFallRate);
    }
    if ((Pad(m_pad).m_pressed & 8) == 8 && std::fabs(m_states[0].m_speed) < s_slowSpeed) {
        input.m_getOff = 1;
    } else {
        input.m_getOff = 0;
    }
}

void TVehicleCarMove::UpdateBoost(float dt) {
    const CarParam &param = Param();
    State &state = m_states[0];
    if (!IsOnGround(state, state.m_wheels[0]) && !IsOnGround(state, state.m_wheels[1])) {
        state.m_boost.m_time = 0.0f;
        state.m_boost.m_charge = 0.0f;
        state.m_boost.m_hold = 0.0f;
    } else if (state.m_boost.m_time > 0.0f) {
        state.m_boost.m_time += dt;
        DrainGauge(dt);
        if (state.m_input.m_horn) {
            state.m_boost.m_time = 0.0f;
            state.m_boost.m_charge = 0.0f;
            state.m_boost.m_hold = 0.0f;
            m_target.m_lampOff = 1;
        }
        if (std::fabs(state.m_boost.m_push.x) < param.field_0x6C || state.m_speed < s_slowSpeed) {
            state.m_boost.m_hold += dt;
            if (state.m_boost.m_hold >= param.field_0x70) {
                state.m_boost.m_time = 0.0f;
                state.m_boost.m_charge = 0.0f;
                state.m_boost.m_hold = 0.0f;
                m_target.m_lampOff = 1;
            }
        }
    } else if (state.m_boost.m_charge >= param.field_0x68) {
        ChargeGauge(dt);
        if (state.m_speed >= param.field_0x64) {
            if (state.m_input.m_accel) {
                if (std::fabs(Pad(m_pad).m_stickX) > 0.4f) {
                    state.m_boost.m_time += dt;
                } else {
                    state.m_boost.m_time = 0.0f;
                    state.m_boost.m_charge = 0.0f;
                    state.m_boost.m_hold = 0.0f;
                }
            }
        } else {
            state.m_boost.m_time = 0.0f;
            state.m_boost.m_charge = 0.0f;
            state.m_boost.m_hold = 0.0f;
        }
    } else {
        DrainGauge(dt);
        if (state.m_speed >= param.field_0x64 && !state.m_input.m_accel) {
            state.m_boost.m_charge += dt;
        } else {
            state.m_boost.m_time = 0.0f;
            state.m_boost.m_charge = 0.0f;
            state.m_boost.m_hold = 0.0f;
        }
    }
}

CarVector TVehicleCarMove::GetForce(Wheel &wheel, const Mtx34f *matrix) {
    State &state = m_states[0];
    float speed = m_states[1].m_speed;
    const CarParam &param = Param();
    Vec3f forward;
    fn_800091EC(&forward, &(const Vec3f &)fn_8040B618(matrix, &Vec3f(0.0f, 0.0f, -1.0f)));
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040B618(matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
    Vec3f right;
    fn_800091EC(&right, &(const Vec3f &)fn_8040B618(matrix, &Vec3f(1.0f, 0.0f, 0.0f)));
    CarVector force;
    CopyVector(V(force), lbl_805E07A0);
    if (IsOnGround(state, wheel)) {
        if (state.m_input.m_brake && speed < s_slowSpeed) {
            fn_800091D0(&V(force), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDA8(&forward, -param.field_0x24),
                                              state.m_input.m_brakeLevel));
        }
        if (state.m_input.m_accel && speed > -s_slowSpeed && speed < param.field_0x18) {
            fn_800091D0(&V(force), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDA8(&forward, param.field_0x20),
                                              state.m_input.m_accelLevel));
        }
    }
    if (IsOnGround(state, wheel)) {
        fn_8040CFF8(&V(force), &(const Vec3f &)((MakeVectorFn)fn_80008E1C)(0.0f, param.field_0x34, 0.0f));
        if (!fn_80014E34(&wheel.m_flags.m_bits, 2) && !m_target.m_lampOff) {
            fn_8040F990(0, &V(wheel.m_velocity), &V(wheel.m_velocity), &right);
        }
    } else {
        fn_8040CFF8(&V(force), &(const Vec3f &)((MakeVectorFn)fn_80008E1C)(0.0f, param.field_0x34, 0.0f));
    }
    return force;
}

void TVehicleCarMove::SlideWheel(float dt, Wheel &wheel, const Vec3f &force, const Vec3f &right) {
    State &state = m_states[0];
    const CarParam &param = Param();
    if (fn_80014E34(&wheel.m_flags.m_bits, 0)) {
        if (m_states[0].m_mode == 2) {
            if (IsOnGround(state, wheel) && !state.m_input.m_boost) {
                if (m_target.m_lampOff) {
                    Vec3f along;
                    Vec3f across;
                    fn_8040F990(&along, &across, &V(wheel.m_velocity), &right);
                    Vec3f forceAlong;
                    Vec3f forceAcross;
                    fn_8040F990(&forceAlong, &forceAcross, &force, &right);
                    fn_804103E8(&V(wheel.m_offset), &along, &forceAlong, param.field_0x30, dt);
                    fn_804103E8(&V(wheel.m_offset), &across, &forceAcross, param.field_0x28, dt);
                    fn_800091D0(&V(wheel.m_velocity), &(const Vec3f &)fn_8040CDD0(&along, &across));
                } else {
                    fn_804103E8(&V(wheel.m_offset), &V(wheel.m_velocity), &force, param.field_0x28, dt);
                }
            } else {
                fn_804103E8(&V(wheel.m_offset), &V(wheel.m_velocity), &force,
                            param.field_0x2C + param.field_0x30 * state.m_input.m_boostLevel, dt);
            }
        } else {
            fn_804103E8(&V(wheel.m_offset), &V(wheel.m_velocity), &force, 5.0f * param.field_0x30, dt);
        }
    } else {
        Vec3f across;
        Vec3f along;
        fn_8040F990(&along, &across, &V(wheel.m_velocity), &Vec3f(0.0f, -1.0f, 0.0f));
        Vec3f forceAcross;
        Vec3f forceAlong;
        fn_8040F990(&forceAlong, &forceAcross, &force, &Vec3f(0.0f, -1.0f, 0.0f));
        fn_804103E8(&V(wheel.m_offset), &across, &forceAcross, 3.75f, dt);
        fn_804103E8(&V(wheel.m_offset), &along, &forceAlong, 0.6667f, dt);
        fn_800091D0(&V(wheel.m_velocity), &(const Vec3f &)fn_8040CDD0(&across, &along));
    }
}

void TVehicleCarMove::TurnWheel(float dt, Wheel &wheel, const Vec3f &force, const Vec3f &right) {
    State &state = m_states[0];
    const CarParam &param = Param();
    if (fn_80014E34(&wheel.m_flags.m_bits, 0)) {
        if (IsOnGround(state, wheel) && !state.m_input.m_boost) {
            Vec3f along;
            Vec3f across;
            fn_8040F990(&along, &across, &V(wheel.m_velocity), &right);
            Vec3f forceAlong;
            Vec3f forceAcross;
            fn_8040F990(&forceAlong, &forceAcross, &force, &right);
            fn_804103E8(&V(wheel.m_offset), &along, &forceAlong, param.field_0x2C, dt);
            fn_804103E8(&V(wheel.m_offset), &across, &forceAcross, param.field_0x28, dt);
            fn_800091D0(&V(wheel.m_velocity), &(const Vec3f &)fn_8040CDD0(&along, &across));
        } else {
            fn_804103E8(&V(wheel.m_offset), &V(wheel.m_velocity), &force,
                        param.field_0x2C + param.field_0x30 * state.m_input.m_boostLevel, dt);
        }
    } else {
        Vec3f across;
        Vec3f along;
        fn_8040F990(&along, &across, &V(wheel.m_velocity), &Vec3f(0.0f, -1.0f, 0.0f));
        Vec3f forceAcross;
        Vec3f forceAlong;
        fn_8040F990(&forceAlong, &forceAcross, &force, &Vec3f(0.0f, -1.0f, 0.0f));
        fn_804103E8(&V(wheel.m_offset), &across, &forceAcross, 3.75f, dt);
        fn_804103E8(&V(wheel.m_offset), &along, &forceAlong, 0.6667f, dt);
        fn_800091D0(&V(wheel.m_velocity), &(const Vec3f &)fn_8040CDD0(&across, &along));
    }
}

float TVehicleCarMove::GetSteerAngle() {
    const CarParam &param = Param();
    float speed = std::fabs(m_states[0].m_speed);
    if (speed < param.field_0x1C) {
        return 0.033333f * (-m_states[0].m_input.m_steer *
                               (param.field_0xC + (1.0f - speed / param.field_0x1C) * (param.field_0x10 - param.field_0xC)));
    }
    return 0.033333f * (-m_states[0].m_input.m_steer * param.field_0xC);
}

void TVehicleCarMove::Run(float dt) {
    State &state = m_states[0];
    const CarParam &param = Param();
    if (m_states[0].m_input.m_getOff) {
        GetOff();
    }
    CarMatrix matrix = ((RotationFn)fn_8040B0C0)(state.m_rotation.z);
    fn_8040B884(&M(matrix), state.m_rotation.x);
    fn_8040B848(&M(matrix), state.m_rotation.y);
    Vec3f forward;
    fn_800091EC(&forward, &(const Vec3f &)fn_8040B618(&M(matrix), &Vec3f(0.0f, 0.0f, -1.0f)));
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040B618(&M(matrix), &Vec3f(0.0f, 1.0f, 0.0f)));
    Vec3f right;
    fn_800091EC(&right, &(const Vec3f &)fn_8040B618(&M(matrix), &Vec3f(1.0f, 0.0f, 0.0f)));
    UpdateBoost(dt);
    CarMatrix inverse = ((RotationFn)fn_8040B138)(-state.m_rotation.y);
    fn_8040B884(&M(inverse), -state.m_rotation.x);
    fn_8040B80C(&M(inverse), -state.m_rotation.z);
    fn_800091D0(&V(state.m_boost.m_push), &(const Vec3f &)fn_8040B618(&M(inverse), &V(state.m_velocity)));
    if (m_states[1].m_boost.m_time > 0.0f) {
        Vec3f force;
        CopyVector(force, lbl_805E07A0);
        float turn = param.field_0xA8 * (dt * (param.field_0x58 * (-state.m_input.m_steer * param.field_0xC)));
        state.m_rotation.y += turn;
        UpdateWheelOffsets();
        m_drive.m_steer = GetSteerAngle();
        if (state.m_input.m_accel) {
            fn_800091D0(&force, &(const Vec3f &)fn_8040CDD0(&(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDA8(&forward, param.field_0x5C),
                                                                         state.m_input.m_accelLevel),
                                             &(const Vec3f &)fn_8040CDA8(&up, param.field_0x34)));
        }
        TurnWheel(dt, state.m_wheels[0], force, right);
        TurnWheel(dt, state.m_wheels[1], force, right);
        state.m_speed = fn_8040CF50(&V(state.m_velocity));
        if (fn_8040CF90(&V(state.m_velocity), &forward) < 0.0f) {
            state.m_speed *= -1.0f;
        }
        if (state.m_boost.m_push.x > 0.0f) {
            SetMotion(4);
        } else {
            SetMotion(3);
        }
        m_drive.m_velocity = state.m_velocity;
        m_drive.m_speed = state.m_speed;
    } else {
        if (m_target.m_lampOff && std::fabs(state.m_boost.m_push.x) < 0.1f) {
            m_target.m_lampOff = 0;
        }
        CarVector rearForce;
        rearForce = GetForce(state.m_wheels[1], &M(matrix));
        float speed = std::fabs(state.m_speed);
        m_drive.m_steer = GetSteerAngle();
        CarMatrix turn = ((MultiplyFn)fn_8001E044)(&M(matrix), &(const Mtx34f &)((RotationFn)fn_8040B138)(m_drive.m_steer));
        CarVector frontForce;
        frontForce = GetForce(state.m_wheels[0], &M(turn));
        if (!IsOnGround(state, state.m_wheels[1]) && IsOnGround(state, state.m_wheels[0])) {
            fn_8040CFF8(&V(rearForce), &V(frontForce));
        } else if (!IsOnGround(state, state.m_wheels[0]) && IsOnGround(state, state.m_wheels[1])) {
            fn_8040CFF8(&V(frontForce), &V(rearForce));
        }
        SlideWheel(dt, state.m_wheels[0], V(frontForce), right);
        SlideWheel(dt, state.m_wheels[1], V(rearForce), right);
        state.m_speed = fn_8040CF50(&V(state.m_velocity));
        if (fn_8040CF90(&V(state.m_velocity), &forward) < 0.0f) {
            state.m_speed *= -1.0f;
        }
        if (state.m_speed < -60.0f) {
            SetMotion(2);
        } else if (state.m_speed > param.field_0x18 * 0.25f) {
            SetMotion(1);
        } else {
            SetMotion(6);
        }
        m_drive.m_velocity = state.m_velocity;
        m_drive.m_speed = state.m_speed;
    }
}

void TVehicleCarMove::UpdateMotion(float dt) {
    const CarParam &param = Param();
    State &state = m_states[0];
    for (int i = 0; i < 4; i++) {
        state.m_model.m_spin[i] += param.field_0x14 * (m_drive.m_speed * dt);
    }
    if (state.m_row != 0) {
        for (int i = 0; i < 3; i++) {
            void *motion = m_car->m_rows[state.m_row][i].m_motion;
            if (motion != 0) {
                float range[2];
                fn_80415D40(motion, 0, range);
                if (range[1] > 0.0f) {
                    state.m_time += param.field_0x54 * (m_drive.m_speed / param.field_0x18);
                    if (state.m_time >= range[1]) {
                        state.m_time = range[0];
                    }
                }
            }
        }
    }
    if (m_motionWait != 0) {
        m_motionWait--;
        if (m_motionWait == 0) {
            m_lastMotion = 0;
        }
    }
    if (m_states[0].m_mode == 2 && m_motionWait == 0 && m_motion != m_lastMotion) {
        boost::shared_ptr<CharInfo> rider = m_car->GetRider().lock();
        switch (m_motion) {
        case 1:
            if (rider.get() != 0) {
                rider.get()->PushCommand(new Player::SetExternalMotionCommand(
                    &UnkCarResource_801B9950::GetInstance()->GetFiles(m_type, 0)->a, 4, 0.0f));
            }
            break;
        case 2:
            if (rider.get() != 0) {
                rider.get()->PushCommand(new Player::SetExternalMotionCommand(
                    &UnkCarResource_801B9950::GetInstance()->GetFiles(m_type, 1)->a, 4, 0.0f));
            }
            break;
        case 3:
            if (rider.get() != 0) {
                rider.get()->PushCommand(new Player::SetExternalMotionCommand(
                    &UnkCarResource_801B9950::GetInstance()->GetFiles(m_type, 2)->a, 4, 0.0f));
            }
            break;
        case 4:
            if (rider.get() != 0) {
                rider.get()->PushCommand(new Player::SetExternalMotionCommand(
                    &UnkCarResource_801B9950::GetInstance()->GetFiles(m_type, 3)->a, 4, 0.0f));
            }
            break;
        case 5:
            if (rider.get() != 0) {
                m_motionWait = 60;
                rider.get()->PushCommand(new Player::SetExternalMotionCommand(
                    &UnkCarResource_801B9950::GetInstance()->GetFiles(m_type, 4)->a, 4, 0.0f));
            }
            break;
        case 6:
            if (rider.get() != 0) {
                rider.get()->PushCommand(new Player::SetExternalMotionCommand(
                    &UnkCarResource_801B9950::GetInstance()->GetFiles(m_type, 5)->a, 4, 0.0f));
            }
            break;
        }
        m_lastMotion = m_motion;
        m_motion = 0;
    }
}

void TVehicleCarMove::CheckFall() {
    Sonicteam::System::Geometry::Line line;
    AssignVector(line.m_start, V(m_states[1].m_position));
    AssignVector(line.m_end, V(m_states[0].m_position));
    if (fn_801BFB70(&line)) {
        fn_801B4CF4(m_attack);
        fn_801B4D60(m_attack);
        SetBodyCollider(0x203, 10, 1.0f, 1.0f, 1.0f);
        fn_801B5FA4(m_car);
    }
}

void TVehicleCarMove::CreateEffects() {
    for (int i = 0; i <= 3; i++) {
        m_effects[i] = boost::shared_ptr<CollisionEffect>(new CollisionEffect());
    }
}

void TVehicleCarMove::UpdatePart(void *part) {
    if (m_states[0].m_wait != m_states[1].m_wait) {
        if (m_states[0].m_wait & 1) {
            fn_80038FC0(part);
        } else {
            fn_80038F94(part);
            SetLamp(m_states[0].m_input.m_boost);
        }
    }
}

void TVehicleCarMove::MoveWheel(float dt, Wheel &wheel, Vec3f *position) {
    const CarParam &param = Param();
    Vec3f velocity;
    fn_800091EC(&velocity, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&V(wheel.m_offset), position));
    if (IsOnGround(m_states[0], wheel)) {
        fn_8040CE04(&velocity, &(const Vec3f &)fn_800091A4(&V(wheel.m_normal), fn_8040CF90(&velocity, &V(wheel.m_normal))));
        fn_8040CFF8(&velocity, &(const Vec3f &)fn_800091A4(&(const Vec3f &)fn_800091A4(&V(wheel.m_normal), param.field_0x38), dt));
    } else {
        velocity.y += param.field_0x38 * dt;
    }
    Vec3f ground;
    SweepWheel(&ground, &V(wheel.m_normal), m_car, position, &velocity, &wheel.m_flags.m_bits, 0, param.m_radius);
    if (fn_8040CF50(&velocity) != 0.0f) {
        fn_8040F990(&V(wheel.m_velocity), 0, &V(wheel.m_velocity), &(const Vec3f &)fn_8040CEE8(&velocity));
    }
}

void TVehicleCarMove::PushWheel(UnkSphere_80021C04 &sphere, CarVector &delta, UnkSphere_80021C04 &other) {
    float radius = sphere.radius + other.radius;
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<Vec3f *>(sphere.center),
                                                       reinterpret_cast<Vec3f *>(other.center)));
    CarV3d move = ToV3d(V(delta));
    CarV3d centre = ToV3d(offset);
    fn_800245C8(&centre, &move, 0, radius);
    fn_800091D0(&V(delta), &(const Vec3f &)fn_800169F8(reinterpret_cast<Vec3f *>(&move)));
    fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &V(delta));
}

void TVehicleCarMove::Move(float dt) {
    State &state = m_states[0];
    State &previous = m_states[1];
    const CarParam &param = Param();
    if (m_states[0].m_wait != 0) {
        state.m_wait--;
    }
    float radius;
    if (param.m_radius > param.field_0x94) {
        radius = param.field_0x94 - s_radiusMargin;
    } else {
        radius = param.m_radius;
    }
    Vec3f rear;
    Vec3f front;
    if (state.m_speed > 0.0f) {
        AssignVector(front, V(previous.m_wheels[0].m_offset));
        MoveWheel(dt, state.m_wheels[0], &front);
        UnkSphere_80021C04 frontSphere;
        fn_80021C04(&frontSphere, &front, radius);
        UnkSphere_80021C04 rearSphere;
        fn_80021C04(&rearSphere, &V(previous.m_wheels[1].m_offset), radius);
        CarVector delta;
        fn_800091EC(&V(delta), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&V(state.m_wheels[1].m_offset), &V(previous.m_wheels[1].m_offset)));
        PushWheel(rearSphere, delta, frontSphere);
        AssignVector(V(state.m_wheels[1].m_offset), *reinterpret_cast<Vec3f *>(rearSphere.center));
        AssignVector(rear, V(previous.m_wheels[1].m_offset));
        MoveWheel(dt, state.m_wheels[1], &rear);
    } else {
        AssignVector(rear, V(previous.m_wheels[1].m_offset));
        MoveWheel(dt, state.m_wheels[1], &rear);
        UnkSphere_80021C04 rearSphere;
        fn_80021C04(&rearSphere, &rear, radius);
        UnkSphere_80021C04 frontSphere;
        fn_80021C04(&frontSphere, &V(previous.m_wheels[0].m_offset), radius);
        CarVector delta;
        fn_800091EC(&V(delta), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&V(state.m_wheels[0].m_offset), &V(previous.m_wheels[0].m_offset)));
        PushWheel(frontSphere, delta, rearSphere);
        AssignVector(V(state.m_wheels[0].m_offset), *reinterpret_cast<Vec3f *>(frontSphere.center));
        AssignVector(front, V(previous.m_wheels[0].m_offset));
        MoveWheel(dt, state.m_wheels[0], &front);
    }
    Vec3f angles(0.0f, 0.0f, 0.0f);
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&front, &rear));
    fn_8003B9E8(&(const Vec3f &)fn_80016284(&direction), &angles.x, &angles.y);
    Vec3f up;
    CopyVector(up, lbl_805E07A0);
    if (IsOnGround(state, state.m_wheels[0]) || IsOnGround(state, state.m_wheels[1])) {
        if (IsOnGround(state, state.m_wheels[0])) {
            fn_8040CFF8(&up, &V(state.m_wheels[0].m_normal));
        } else if (IsOnGround(state, state.m_wheels[1])) {
            fn_8040CFF8(&up, &V(state.m_wheels[1].m_normal));
        }
        fn_8040CE74(&up);
    } else {
        AssignVector(up, V(state.m_up));
    }
    CarMatrix pitch = ((RotationFn)fn_8040B1B0)(-angles.x);
    fn_8040B848(&M(pitch), -angles.y);
    fn_800091D0(&up, &(const Vec3f &)fn_8040B618(&M(pitch), &up));
    float unused;
    fn_8003B8F4(&up, &unused, &angles.z);
    if (!IsOnGround(state, state.m_wheels[0]) && !IsOnGround(state, state.m_wheels[1])) {
        angles.z = fn_8004924C(0.0f, angles.z, 16.0f, 1.0f);
    }
    CarMatrix matrix = ((RotationFn)fn_8040B138)(angles.y);
    fn_8040B884(&M(matrix), angles.x);
    fn_8040B80C(&M(matrix), angles.z);
    fn_800493C4(&V(state.m_rotation), &M(matrix));
    fn_800091D0(&V(state.m_up), &(const Vec3f &)fn_8040B618(&M(matrix), &Vec3f(0.0f, 1.0f, 0.0f)));
    Vec3f back;
    fn_800091EC(&back, &(const Vec3f &)fn_8040B618(&M(matrix), &Vec3f(0.0f, 0.0f, -1.0f)));
    fn_800091D0(&V(state.m_position), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&front, &rear), 0.5f));
    fn_8040CFF8(&V(state.m_position), &(const Vec3f &)fn_800091A4(&(const Vec3f &)fn_80016284(&V(state.m_up)), param.m_radius));
    UpdateWheelOffsets();
    if (0.0f != dt) {
        float rate = 1.0f / dt;
        fn_800091D0(&V(state.m_velocity), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&V(state.m_position), &V(previous.m_position)), rate));
        state.m_speed = fn_8040CF50(&V(state.m_velocity));
        if (fn_8040CF90(&V(state.m_velocity), &back) < 0.0f) {
            state.m_speed *= -1.0f;
        }
        fn_800091D0(&V(state.m_wheels[0].m_velocity), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&V(state.m_wheels[0].m_velocity),
                                                             &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                                                                             &V(state.m_wheels[0].m_offset),
                                                                                             &V(previous.m_wheels[0].m_offset)),
                                                                                         rate)),
                                 0.5f));
        fn_800091D0(&V(state.m_wheels[1].m_velocity), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&V(state.m_wheels[1].m_velocity),
                                                             &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                                                                             &V(state.m_wheels[1].m_offset),
                                                                                             &V(previous.m_wheels[1].m_offset)),
                                                                                         rate)),
                                 0.5f));
    }
    AssignVector(V(state.m_model.m_position), V(state.m_position));
    state.m_model.m_rotation.x = fn_8004924C(state.m_model.m_rotation.x, state.m_rotation.x, 2.0f, 1.0f);
    state.m_model.m_rotation.y = state.m_rotation.y;
    state.m_model.m_rotation.z = fn_8004924C(state.m_model.m_rotation.z, state.m_rotation.z, 2.0f, 1.0f);
    float turn = state.m_rotation.y - previous.m_rotation.y;
    if (turn < -lbl_805FAD38) {
        turn += 2.0f * lbl_805FAD38;
    } else if (turn > lbl_805FAD38) {
        turn -= 2.0f * lbl_805FAD38;
    }
    float roll = param.field_0x98 * turn;
    if (previous.m_boost.m_time > 0.0f) {
        float push = state.m_boost.m_push.x * param.field_0x9C;
        roll += push;
    }
    if (roll < -param.field_0xA0) {
        roll = -param.field_0xA0;
    } else if (roll > param.field_0xA0) {
        roll = param.field_0xA0;
    }
    state.m_roll = fn_8004924C(roll, state.m_roll, 16.0f, 1.0f);
    for (int i = 0; i <= 1; i++) {
        if (fn_80014E34(&state.m_wheels[i].m_flags.m_bits, 0)) {
            if (state.m_wheels[i].m_air >= s_bounceTime) {
                m_drive.m_bounced = 1;
                state.m_wheels[i].m_spring = previous.m_wheels[i].m_velocity.y * s_bounceScale;
                if (state.m_wheels[i].m_spring < -s_bounceLimit) {
                    state.m_wheels[i].m_spring = -s_bounceLimit;
                }
            }
            state.m_wheels[i].m_air = 0.0f;
        } else {
            state.m_wheels[i].m_air += dt;
        }
    }
    if (m_drive.m_bounced && !IsOnGround(state, state.m_wheels[0]) && !IsOnGround(state, state.m_wheels[1])) {
        fn_801B6510(m_car, -5.0f);
        state.m_wait = 60;
        fn_801D560C(fn_80053164(), 0x202F, 0, 0);
        if (fn_801B64F8(m_car) < 0.0f) {
            m_car->SetField98Float(0.0f);
        }
    }
}

void TVehicleCarMove::UpdateSpring() {
    State &state = m_states[0];
    const CarParam &param = Param();
    for (int i = 0; i < 2; i++) {
        float spring = state.m_wheels[i].m_spring;
        float old = state.m_wheels[i].m_springOld;
        state.m_wheels[i].m_springOld = spring;
        state.m_wheels[i].m_spring = (spring - old) * s_springDamp + -spring * s_springPull;
    }
    state.m_lift = 0.5f * (state.m_wheels[0].m_spring + state.m_wheels[1].m_spring);
    Vec3f tilt(0.0f, state.m_wheels[0].m_spring - state.m_wheels[1].m_spring, 2.0f * -param.field_0x94);
    Vec3f angles(0.0f, 0.0f, 0.0f);
    fn_8003B9E8(&tilt, &angles.x, &angles.y);
    state.m_pitch = angles.x;
}

int TVehicleCarMove::GetDamage() {
    int damage = 0;
    const CarParam &param = Param();
    if (m_states[0].m_mode == 2) {
        damage = (int)(((float)param.field_0x4 * (2.0f * std::fabs(m_states[0].m_speed))) / param.field_0x18);
        if (std::fabs(m_states[0].m_speed) > 0.2f * s_slowSpeed) {
            damage++;
        }
    }
    return damage;
}

void TVehicleCarMove::OnDamaged(AttackCallbackParam &param) {
    if (m_states[0].m_wait == 0) {
        fn_801B6510(m_car, -CarParam_(param).m_damage);
        if (fn_801B64F8(m_car) > 0.0f) {
            CarParam_(param).m_result = 1;
            fn_801D560C(fn_80053164(), 0x202F, 0, 0);
        } else {
            CarParam_(param).m_result = 3;
            m_car->SetField98Float(0.0f);
        }
        m_states[0].m_wait = 60;
    } else {
        CarParam_(param).m_result = 5;
    }
}

void TVehicleCarMove::OnAttackHit(AttackCallbackParam &param) {
    if (!fn_80053538(reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(param.field_0x8))) {
        if (fn_803553E8(m_hits, param.field_0x8)) {
            CarParam_(param).m_ignored = 1;
        }
        fn_803552DC(m_hits, param.field_0x8);
        if (CarParam_(param).m_ignored) {
            return;
        }
    }
    m_drive.m_damage = GetDamage();
    CarParam_(param).m_damage = m_drive.m_damage;
}

void TVehicleCarMove::OnAttackEnd(AttackCallbackParam &param) {
    m_drive.m_hit = 1;
    switch (CarParam_(param).m_result) {
    case 1:
        fn_801B6510(m_car, -m_drive.m_damage);
        if (fn_801B64F8(m_car) < 0.0f) {
            m_car->SetField98Float(0.0f);
        }
        m_drive.m_hit = 0;
        break;
    default:
        m_drive.m_hit = 1;
        break;
    }
}

void TVehicleCarMove::UpdateStuck(float dt) {
    UpdateSpring();
    State &state = m_states[0];
    if (fn_80014E34(&state.m_wheels[0].m_flags.m_bits, 0) && fn_80014E34(&state.m_wheels[1].m_flags.m_bits, 0) &&
        !IsOnGround(state, state.m_wheels[0]) && !IsOnGround(state, state.m_wheels[1])) {
        state.m_stuck += dt;
        if (state.m_stuck >= s_stuckTime) {
            m_car->SetField98Float(0.0f);
        }
    }
}

void TVehicleCarMove::BeginFrame() {
    m_states[1] = m_states[0];
    fn_80355260(m_hits, 1);
    fn_8035511C(m_hits);
}

void TVehicleCarMove::UpdateGround() {
    if (m_frame < 2) {
        fn_800091D0(&V(m_states[0].m_velocity), &Vec3f(0.0f, 0.0f, 0.0f));
    }
    float speed = std::fabs(m_states[0].m_speed);
    if (std::fabs(m_states[1].m_speed) - speed > 100.0f) {
        SetMotion(5);
    }
    const CarParam &param = Param();
    int level = (int)((fn_801B64F8(m_car) / (float)param.m_life) / 0.33333334f);
    if (level >= 3) {
        level = 2;
    }
    m_states[0].m_row = 2 - level;
    int corner;
    Vec3f offset;
    if (m_type == 3) {
        switch (corner = *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) & 1) {
        case 0:
            offset.x = 0.0f;
            offset.y = -param.m_size->y;
            offset.z = -param.m_size->z;
            break;
        case 1:
            offset.x = 0.0f;
            offset.y = -param.m_size->y;
            offset.z = param.m_size->z;
            break;
        }
    } else {
        switch (corner = *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) & 3) {
        case 0:
            offset.x = -param.m_size->x;
            offset.y = -param.m_size->y;
            offset.z = -param.m_size->z;
            break;
        case 1:
            offset.x = param.m_size->x;
            offset.y = -param.m_size->y;
            offset.z = -param.m_size->z;
            break;
        case 2:
            offset.x = -param.m_size->x;
            offset.y = -param.m_size->y;
            offset.z = param.m_size->z;
            break;
        case 3:
            offset.x = param.m_size->x;
            offset.y = -param.m_size->y;
            offset.z = param.m_size->z;
            break;
        }
    }
    CarMatrix matrix = ((TranslationFn)fn_8040B034)(&V(m_states[0].m_position));
    fn_8040B80C(&M(matrix), m_states[0].m_rotation.z);
    fn_8040B884(&M(matrix), m_states[0].m_rotation.x);
    fn_8040B848(&M(matrix), m_states[0].m_rotation.y);
    Vec3f start;
    fn_800091EC(&start, &(const Vec3f &)fn_8040B67C(&M(matrix), &offset));
    u32 kind;
    float line[6];
    m_target.m_grounded = LineTest(
        *fn_80020A20(
            line,
            &(const Vec3f &)fn_8040CDD0(&start, &(const Vec3f &)fn_8040CDA8(&V(m_states[0].m_up), param.m_size->y)),
            &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&start, &Vec3f(0.0f, 1.0f, 0.0f))),
        &V(m_target.m_ground), &kind);
    if (m_effects[corner].get() != 0) {
        if (m_target.m_grounded && std::fabs(m_states[0].m_speed) > s_slowSpeed) {
            fn_803444E0(m_effects[corner].get(), &V(m_target.m_ground), &V(m_states[0].m_up), 0, kind);
        } else {
            fn_803444BC(m_effects[corner].get());
        }
    }
}

void TVehicleCarMove::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    const CarParam &param = Param();
    if (position != 0) {
        AssignVector(V(m_states[0].m_position), *position);
        AssignVector(V(m_states[0].m_model.m_position), *position);
    }
    if (rotation != 0) {
        m_states[0].m_rotation.x = rotation->x;
        m_states[0].m_rotation.y = rotation->y;
        m_states[0].m_rotation.z = rotation->z;
        m_states[0].m_model.m_rotation.x = rotation->x;
        m_states[0].m_model.m_rotation.y = rotation->y;
        m_states[0].m_model.m_rotation.z = rotation->z;
    }
    m_states[0].m_row = 0;
    m_states[0].m_speed = 0.0f;
    AssignVector(V(m_states[0].m_velocity), lbl_805E07A0);
    m_states[0].m_up.x = 0.0f;
    m_states[0].m_up.y = 1.0f;
    m_states[0].m_up.z = 0.0f;
    m_states[0].m_pitch = m_states[0].m_roll = 0.0f;
    m_states[0].m_stuck = 0.0f;
    for (int i = 0; i <= 1; i++) {
        fn_800091D0(&V(m_states[0].m_wheels[i].m_normal), &Vec3f(0.0f, 1.0f, 0.0f));
        AssignVector(V(m_states[0].m_wheels[i].m_velocity), lbl_805E07A0);
        m_states[0].m_wheels[i].m_spring = 0.0f;
        m_states[0].m_wheels[i].m_springOld = 0.0f;
        m_states[0].m_wheels[i].m_air = 0.0f;
        fn_80014E10(&m_states[0].m_wheels[i].m_flags.m_bits);
    }
    m_states[1] = m_states[0];
    UpdateWheelOffsets();
    SetLamp(false);
    m_frame = 0;
    fn_801B6570(m_car, param.m_life);
    m_car->SetField98Float(param.m_life);
    m_target.m_grounded = 0;
    m_target.m_lampOff = 0;
}

void TVehicleCarMove::CreateColliders() {
    const CarParam &param = Param();
    boost::weak_ptr<CharInfo> self = boost::static_pointer_cast<CharInfo>(m_car->GetSelf().lock());
    CarVector size(*param.m_size);
    Vec3f offset;
    offset.z = 0.0f;
    offset.x = 0.0f;
    offset.y = size.y;
    size.x *= param.field_0x7C;
    size.z *= param.field_0x7C;
    m_body2 = fn_800C7C34(fn_80021B08(), self, &V(size), 0, 2, 0, &offset, &lbl_805E07B8, 0);
    fn_801B63D4(m_car, m_body2);
    m_attack = new UnkBirdCollision_801B4EBC();
    fn_801B4DD0(m_attack, m_car);
    SetBodyCollider(0x203, 10, 1.0f, 1.0f, 1.0f);
}

void TVehicleCarMove::SetBodyCollider(int flags, int arg, float sx, float sy, float sz) {
    const CarParam &param = Param();
    boost::weak_ptr<CharInfo> self = boost::static_pointer_cast<CharInfo>(m_car->GetSelf().lock());
    CarVector size(*param.m_size);
    Vec3f offset;
    offset.z = 0.0f;
    offset.x = 0.0f;
    offset.y = param.m_size->y;
    size.x *= sx;
    size.y *= sy;
    size.z *= sz;
    m_bodyCollider = fn_800C7C34(fn_80021B08(), self, &V(size), flags, arg, 0, &offset, &lbl_805E07B8, 0);
    fn_801B6470(m_car, m_bodyCollider);
}

void TVehicleCarMove::UpdateWheelOffsets() {
    const CarParam &param = Param();
    CarMatrix matrix = ((TranslationFn)fn_8040B034)(&V(m_states[0].m_position));
    fn_8040B80C(&M(matrix), m_states[0].m_rotation.z);
    fn_8040B884(&M(matrix), m_states[0].m_rotation.x);
    fn_8040B848(&M(matrix), m_states[0].m_rotation.y);
    fn_800091D0(&V(m_states[0].m_wheels[0].m_offset), &(const Vec3f &)fn_8040B67C(&M(matrix), &Vec3f(0.0f, param.m_radius, -param.field_0x94)));
    fn_800091D0(&V(m_states[0].m_wheels[1].m_offset), &(const Vec3f &)fn_8040B67C(&M(matrix), &Vec3f(0.0f, param.m_radius, param.field_0x94)));
}

void *TVehicleCarMove::GetNode(int index) {
    const CarModelDesc &desc = ModelDesc();
    RpClump *clump;
    if (desc.m_nodes[index].m_inParts) {
        clump = m_car->m_parts[desc.m_nodes[index].m_part].m_clump;
    } else {
        clump = m_car->m_rows[m_states[0].m_row][desc.m_nodes[index].m_part].m_clump;
    }
    if (clump != 0) {
        return fn_80038E84(*reinterpret_cast<void **>(reinterpret_cast<u8 *>(clump) + 4), desc.m_nodes[index].m_node);
    }
    return 0;
}

void TVehicleCarMove::SetLamp(bool on) {
    const CarModelDesc &desc = ModelDesc();
    RpClump *clump;
    if (desc.m_nodes[10].m_inParts) {
        clump = m_car->m_parts[desc.m_nodes[10].m_part].m_clump;
    } else {
        clump = m_car->m_rows[m_states[0].m_row][desc.m_nodes[10].m_part].m_clump;
    }
    if (clump != 0) {
        void *node = GetNode(10);
        if (node != 0) {
            if (on) {
                fn_80038F2C(clump, node, 0);
            } else {
                fn_80038F60(clump, node, 0);
            }
        }
    }
}

void TVehicleCarMove::GetOff() {
    fn_801B6030(m_car, 0);
    SetCamera(1, 0);
    fn_801B4CF4(m_attack);
    fn_801B4D60(m_attack);
    SetBodyCollider(0x203, 10, 1.0f, 1.0f, 1.0f);
}

int TVehicleCarMove::ToType(int kind) {
    switch (kind) {
    case 1:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    }
    return 0;
}

/* An unused function (name and meaning unknown): retail pools a 0.1f here, between the constants of
 * SetPlacement and those of the sweep, where no linked function uses it first; the linker drops
 * this copy (see docs/research-notes.md). */
float TVehicleCarMove::GetStep() {
    return 0.1f;
}

/* Accepts a candidate hit of the tests (name inferred). */
static bool AcceptHit(UnkCarCandidate_80021934 *candidate) {
    const UnkCarSweepFilter_801B9ADC *filter = candidate->m_filter;
    u32 mask = filter->m_mask;
    if (candidate->m_collider != 0) {
        if (candidate->m_collider == filter->m_other.get()) {
            return false;
        }
        if (candidate->m_collider == filter->m_body.get()) {
            return false;
        }
        if (candidate->m_attributes & 0x200000) {
            return false;
        }
    }
    return (candidate->m_attributes & mask) != 0;
}

/* The attack collision of a collider attached to a character (see Game/BulletUtil.cpp; an inline
 * function of another unit, out of line). */
boost::weak_ptr<CharaColliAttack> AttackOf(CharaColliBaseCharInfo *collider);

/* Sweeps a sphere of radius around position along velocity through the world, sliding along what
 * it touches (along the slope when slide is set) at most count times; chara gets the character
 * last touched, and normal and ground the ground the sphere lands on (bit 0 of flags; bit 2 when
 * it touched something else). Whether it touched anything (name inferred). */
static bool Sweep(TVehicleBase *vehicle, u32 mask, Vec3f *position, Vec3f *velocity, bool slide,
                  boost::weak_ptr<CharInfo> *chara, int count, u32 *flags, Vec3f *normal, Vec3f *ground,
                  float radius) {
    void *world = fn_80020D40();
    UnkCarSweepFilter_801B9ADC filter;
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, position, radius);
    CollisionResultList *previous = 0;
    Vec3f move;
    fn_800091EC(&move, velocity);
    Vec3f groundNormal;
    fn_800091EC(&groundNormal, &lbl_805E07A0);
    bool landed = false;
    bool touched = false;
    filter.m_mask = mask;
    filter.m_vehicle = vehicle;
    if (vehicle != 0) {
        filter.m_body = fn_801B6350(vehicle);
        filter.m_other = fn_801B6380(vehicle);
    }
    bool hit = false;
    bool again;
    CollisionResultList *hits;
    do {
        again = false;
        Vec3f start;
        fn_800091EC(&start, &move);
        hits = fn_80021934(world, &sphere, &move, 3, 1, previous, AcceptHit, &filter, 0);
        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &move);
        if (chara != 0) {
            chara->reset();
        }
        if (hits != 0) {
            hit = true;
            Vec3f remain;
            fn_800091EC(&remain, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&start, &move));
            Vec3f push(0.0f, 0.0f, 0.0f);
            float pushes = 0.0f;
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(hits)); it.m_node != fn_8001EDEC(hits);
                 fn_80027374(&it, 0)) {
                UnkColliContactNode_800587A0 *node = it.m_node;
                void *object = node->m_object;
                if (!IsSpecialSurface(reinterpret_cast<UnkCollision_80021E24 *>(node)) &&
                    reinterpret_cast<UnkCollision_80021E24 *>(node)->m_surface->m_id == 1) {
                    boost::weak_ptr<CharaColliBase> weakCollider = fn_800587A0(node);
                    boost::shared_ptr<CharaColliBase> collider = weakCollider.lock();
                    if (collider.get() != 0) {
                        CharaColliBaseCharInfo *charCollider = dynamic_cast<CharaColliBaseCharInfo *>(collider.get());
                        if (charCollider != 0) {
                            boost::weak_ptr<CharInfo> weakOwner = fn_800586EC(charCollider);
                            boost::shared_ptr<CharInfo> owner = weakOwner.lock();
                            if (owner.get() != 0 && owner.get()->GetKind() == 0x27) {
                                boost::weak_ptr<CharaColliAttack> weakAttack = AttackOf(charCollider);
                                boost::shared_ptr<CharaColliAttack> attack = weakAttack.lock();
                                if (attack.get() != 0) {
                                    vehicle->SetField98Float(0.0f);
                                    continue;
                                }
                            }
                        }
                    }
                }
                Vec3f hitNormal;
                fn_800091EC(&hitNormal, fn_800266D4(object));
                if (normal != 0) {
                    if (fn_8040CF90(&hitNormal, &Vec3f(0.0f, 1.0f, 0.0f)) >= 0.5f && fn_8040CF90(&hitNormal, normal) >= 0.5f) {
                        landed = true;
                        fn_8040CFF8(&groundNormal, &hitNormal);
                        if (ground != 0) {
                            fn_800091D0(&*ground, &(const Vec3f &)(*fn_8001E9BC(object)));
                        }
                    } else {
                        touched = true;
                    }
                }
                if (fn_8040CF90(&hitNormal, &remain) <= 0.0f) {
                    fn_8040CE04(&remain, &(const Vec3f &)fn_80093230(&hitNormal));
                }
                if (fn_8040CF90(&hitNormal, velocity) <= 0.0f) {
                    Vec3f along;
                    Vec3f across;
                    fn_8040F990(&along, &across, velocity, &hitNormal);
                    if (vehicle != 0) {
                        vehicle->vf_0x40(node);
                    }
                    if (fn_8040CF90(&across, velocity) <= 0.0f) {
                        fn_8040CE60(velocity);
                    } else if (slide) {
                        float slope = fn_8040CF90(&(const Vec3f &)fn_8040CEE8(velocity), &(const Vec3f &)fn_8040CEE8(&along));
                        Vec3f turn;
                        float half = 0.5f * slope;
                        fn_800091EC(&turn, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_80016284(&along), 1.0f + half));
                        fn_8040CFF8(velocity, &turn);
                    } else {
                        AssignVector(*velocity, across);
                    }
                }
                fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &(const Vec3f &)fn_800091A4(&hitNormal, s_skin));
                if (fn_80099080(object) == 1) {
                    Vec3f offset;
                    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                             reinterpret_cast<Vec3f *>(sphere.center), fn_8001E9BC(object)));
                    float distance = fn_8040CF90(&offset, &hitNormal);
                    if (distance >= 0.0f && fn_8040CF2C(&offset) < sphere.radius * sphere.radius) {
                        Vec3f out;
                        fn_800091EC(&out, &(const Vec3f &)fn_8040CDA8(&hitNormal, sphere.radius - distance));
                        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &out);
                        fn_8040CFF8(&push, &out);
                        pushes += 1.0f;
                    }
                }
                boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
                boost::weak_ptr<CharInfo> owner;
                if (collider.get() != 0) {
                    boost::shared_ptr<CharaColliBaseCharInfo> charCollider =
                        boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(collider);
                    owner = fn_800586EC(charCollider.get());
                }
                if (chara != 0) {
                    *chara = owner;
                }
            }
            if (pushes > 1.0f) {
                Vec3f average;
                fn_800091EC(&average, &(const Vec3f &)fn_8040CDA8(&push, 1.0f / pushes));
                fn_8040CE04(reinterpret_cast<Vec3f *>(sphere.center), &average);
            }
            if (fn_8040CF2C(&remain) > 0.001f) {
                again = true;
                AssignVector(move, remain);
            }
            if (previous != 0) {
                fn_80026FB4(previous, 1);
            }
            previous = hits;
            hits = 0;
        }
        count--;
    } while (again && count > 0);
    if (hits != 0) {
        fn_80026FB4(hits, 1);
    }
    if (previous != 0) {
        fn_80026FB4(previous, 1);
    }
    AssignVector(*position, *reinterpret_cast<Vec3f *>(sphere.center));
    if (landed) {
        fn_80014E1C(flags, 0);
        if (normal != 0) {
            fn_800091D0(&*normal, &(const Vec3f &)fn_8040CEE8(&groundNormal));
        }
    }
    if (touched) {
        fn_80014E1C(flags, 2);
    }
    return hit;
}

bool TVehicleCarMove::SweepWheel(Vec3f *ground, Vec3f *normal, TVehicleBase *vehicle, Vec3f *position,
                                 Vec3f *velocity, u32 *flags, const Vec3f *up, float radius) {
    boost::weak_ptr<CharInfo> chara;
    fn_8006127C(flags, 0);
    fn_8006127C(flags, 2);
    if (up != 0) {
        AssignVector(*normal, *up);
    } else {
        fn_800091D0(normal, &Vec3f(0.0f, 1.0f, 0.0f));
    }
    return Sweep(vehicle, 0x1A93, position, velocity, fn_80014E34(flags, 1), &chara, 5, flags, normal, ground,
                 radius);
}

bool TVehicleCarMove::IsOnGround(State &state, Wheel &wheel) {
    const CarParam &param = Param();
    if (fn_80014E34(&wheel.m_flags.m_bits, 0)) {
        float dot = fn_8040CF90(&V(wheel.m_normal), &V(state.m_up));
        if (dot >= std::cos(param.m_slope)) {
            return true;
        }
    }
    return false;
}

bool TVehicleCarMove::LineTest(UnkDebugLine_80020A20 line, Vec3f *position, u32 *kind) {
    void *world = fn_80020D40();
    UnkCarSweepFilter_801B9ADC filter;
    filter.m_mask = 0x1A93;
    filter.m_body = fn_801B6350(m_car);
    filter.m_other = fn_801B6380(m_car);
    filter.m_vehicle = 0;
    UnkCollision_80021E24 *hit = fn_80021E24(world, &line, 3, 1, AcceptHit, &filter);
    if (hit != 0) {
        if (hit->m_object != 0) {
            fn_800091D0(&*position, &(const Vec3f &)(*fn_8001E9BC(hit->m_object)));
            *kind = hit->m_surface->m_flags >> 28;
        }
        fn_800265A4(hit, 1);
        return true;
    }
    return false;
}

/* A variable of the debug editor for one of the tuning values (name and use unknown). Nothing calls
 * it: the linker drops it, but it leaves VariableBasic<float>'s RTTI and vtable in .data, as retail
 * has them. */
Sonicteam::Gindows::Utility::VariableEditor::Variable *CreateTuningVariable(float *value) {
    return new Sonicteam::Gindows::Utility::VariableEditor::VariableBasic<float>(value);
}
