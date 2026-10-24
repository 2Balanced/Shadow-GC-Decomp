/* Coaster (class name from RTTI): the Coaster gadget of Circus Park, a roller coaster car
 * (COASTER.BIN) that runs along a path of the stage. A player who touches the car is hung in its
 * seat (TouchHangCommand) and rides it along path "PATH_NO" at "MOVE_SPPED m/sec", while a user
 * camera mode follows the car; with the stage's input enabled the player tilts the car. At the end
 * of the path the player is launched at "LAUNCH_SPD m/sec", tilted by "LAUNCH_ANG deg", and ignores
 * input for "LAUNCH_NO_CTRL_TIME sec"; the car then runs back, or, on stages that say so, breaks
 * up. Each stage that has coasters has a record of its paths and camera placement. File name
 * inferred from the class. Method names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first).
 *
 * The constructor addresses its three member pointer constants and the vtable through the start of
 * .data, so the linker keeps that section whole: the orphan RTTI base lists of the two first
 * headers lead it, and zeroed copies of weak vtables close it.
 *
 * Not yet matching (two functions; everything else, including the data, matches):
 * - OnCameraUpdate: retail loads m_chara's pointer into a saved register before each
 *   fn_8040B67C call and passes it to fn_8040CDD0 afterwards; GC/2.7 loads it after the call
 *   (also when it is first copied to a local, which the compiler propagates past the call). No
 *   source shape tried reproduces retail's order; probably the newer compiler of
 *   docs/research-notes.md.
 * - Behave: in the running-back case retail keeps the stage record's pointer in r5 and loads
 *   its flag before m_backward; GC/2.7 uses r8 and loads the flag last (4 instructions, register
 *   allocation only).
 * The zeroed vtables at the end of .data keep placeholder names in symbols.txt while the unit is
 * not linked: with their real names the assembly of this range would resolve the live
 * sp_counted_base vtable to its own zeroed copy. Give them their names (scope:local) when the unit
 * links, as FootingMovable has them. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CameraModeUser.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/PathCtrl.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* MSL's std::cos(float) and std::fabs(float), which the game calls out of line (0x8000F3B0,
 * 0x8000A980). They are declared here rather than through <cmath>, whose sqrtf adds two double
 * constants to .sdata2 that this unit lacks (as in Fan.cpp). */
namespace std {
float cos(float x);
float fabs(float x);
}
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

using SonicteamUSA::System::PathCtrl;

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for PathCtrl (boost 1.32's definition, as in SetWeapon.cpp). */
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * ElecCircuitColli). */
class GadgetCharCollision {
  public:
    /* Sets and clears flag bit 1 (names inferred: the handle ignores contacts while it is set). */
    void TurnOff() {
        m_flags |= 2;
    }

    void TurnOn() {
        m_flags &= ~2;
    }

    /* Sets flag bit 0 and the float at 0x30 (name and meaning inferred). */
    void LetGo(float time) {
        m_flags |= 1;
        field_0x30 = time;
    }

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
    /* 0x30 */ float field_0x30;
};

/* AccelerateInputCommand (command type 1) is declared in Game/CharCommand.h. */

/* The parameters of a debris spawn (see GadgetDebris.h; the fields set here as in ArkCannon). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* An entry of a model's sound table (name inferred). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_soundId; /* passed to the sound calls as 16 bits */
};

/* The game state block in .bss (only the members used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x28];
    /* 0x028 */ float field_0x28; /* scales the tilt input (meaning unknown) */
    /* 0x02C */ u8 field_0x2C[0xAC - 0x2C];
    /* 0x0AC */ s32 m_stage; /* (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

class UnkPathManager_8001B494;
class UnkModelTable_80208888;

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Launches the debris described by param (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* The character manager's character by id, and the player's id (see SecretDoor.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
extern int lbl_805EE588;
/* Whether the collision event involves a player (behaviour inferred). */
bool fn_8015D2EC(UnkColliContacts_8001EE04 *contacts);
/* Whether a path follower is set up (behaviour inferred: its body tests a weak count kept at 0x4
 * of the follower). */
bool fn_800ABDCC(PathCtrl *path);

/* Owner helpers (behaviour inferred): the owner's flag word, writing the parameter block back,
 * whether the owner went away or is out of range, its range, and telling it the gadget is gone. */
void fn_800C9E70(UnkPlacement_800C9CD0 *owner, u32 *flags);
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
float fn_800C9938(UnkPlacement_800C9CD0 *owner);
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* Model helpers (behaviour inferred, as in FootingMovable). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
void fn_8016082C(GadgetModelInfo *model, int arg);
void fn_80162790(GadgetModelInfo *model);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The model loader, a model's sound table entry, and playing a sound (behaviour inferred). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
UnkSoundEntry_80208888 *fn_80208848(UnkModelTable_80208888 *table, int index);
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *charInfo, int arg3, int arg4);

/* The collider manager, a sphere collider for position, the handle around it, and the handle's
 * debug draw and update (behaviour inferred, as in UpDownReel). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, int flags,
                                              int arg3, int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Creates a target following owner (behaviour inferred). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);

/* Paths (behaviour inferred, as in FootingMovable): the path manager, finding path id, restarting,
 * the position, the rotation, and moving a character info distance along a path (reporting whether
 * it reached the end). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, u32 id);
void fn_80018304(PathCtrl *path);
Vec3f fn_80018634(PathCtrl *path);
Vec3f fn_80018434(PathCtrl *path, int arg1, int arg2);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int backward,
                 GadgetCharInfo *charInfo, Vec3f *rotation, int arg5, float distance);

/* True when value is (nearly) zero (behaviour inferred). */
bool fn_8000B044(float value);
/* value clamped to [low, high] (behaviour inferred). */
const float &fn_80070460(const float &value, const float &low, const float &high);
/* Matrix helpers (behaviour inferred): the Euler angles of a rotation matrix, building the
 * rotation about axis by angle, a * b, and inverting a rotation in place. */
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
void fn_800493C4(Vec3f *rotation, const Mtx34f *matrix);
void fn_800498A0(Mtx34f *matrix, const Vec3f *axis, float angle);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_8040BCD4(Mtx34f *matrix);
/* Vector helpers (behaviour inferred): a + b, vector += value, the vector turned about x and about
 * y, and normalising it. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
float fn_8040CE74(Vec3f *vector);
/* Passed to the debug line's draw call (meaning unknown). */
extern u32 lbl_805F1370;
}

/* Helpers declared as in GadgetUtil.h, which this unit does not include: the original's pi is a
 * constant here (loads of it are scheduled across stores), as MathConstants.h declares it. */
extern "C" {
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void *fn_80053164(void);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CE38(Vec3f *vector, float scale);
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
void fn_802E0B58(void *segment, u32 *arg1);
}

inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Turns current toward target by at most step (see AngleUtil.h; the copy the game keeps is
 * UpDownReel's). */
float TurnAngleToward(float target, float current, float step);

/* An empty character pointer (meaning inferred; several gadgets pass it to the collider setup). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
/* fn_80008E1C called as a constructor returning the vector it built. */
typedef Vec3f (*VectorCtorFn)(float x, float y, float z);

/* Copies the yaw of a rotation (name inferred). An inline function in the original: retail calls
 * the smart pointer's operator-> out of line before it. */
inline void GetYaw(float *yaw, const Vec3f &rotation) {
    *yaw = rotation.y;
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Converts the parameters' metres to world units (name inferred). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* The path ids of each stage's coasters (names inferred). */
static const int lbl_804BED00[] = {30, 31, 32, 33};
static const int lbl_805F4228[] = {5, 6};
static const int lbl_804BED10[] = {1, 2, 7};
static const int lbl_804BED1C[] = {1, 2, 3, 4, 5};
static const int lbl_804BED30[] = {30, 31, 32};

/* What a stage's coasters do (layout from the uses; names inferred). */
struct CoasterStageConfig {
    /* 0x00 */ s32 stage;
    /* 0x04 */ float seatHeight; /* the seat's height in the car */
    /* 0x08 */ u32 pathCount;
    /* 0x0C */ const int *paths;
    /* 0x10 */ u8 levelCamera;   /* the camera and the car only turn about y */
    /* 0x11 */ u8 returnLoops;   /* passed when the car runs back */
    /* 0x12 */ u8 animate;       /* the car plays its motion while it rides */
    /* 0x13 */ u8 tiltInput;     /* the rider's input tilts the car */
    /* 0x14 */ u8 breakAtEnd;    /* the car breaks up at the end of the path */
    /* 0x18 */ float cameraSide; /* the camera's offset from the seat */
    /* 0x1C */ float cameraBack;
    /* 0x20 */ float lookHeight; /* added to the seat height for the point looked at */
    /* 0x24 */ float blendTime;  /* of the camera mode change */
    /* 0x28 */ const char *debrisModel;
};
SIZE_ASSERT(CoasterStageConfig, 0x2C);

static const CoasterStageConfig lbl_804BED6C[5] = {
    {9, -30.0f, 4, lbl_804BED00, 1, 1, 0, 1, 0, 20.0f, 100.0f, 10.0f, 2.0f, 0},
    {0xB, -23.0f, 2, lbl_805F4228, 0, 1, 1, 1, 0, 10.0f, 110.0f, 20.0f, 2.0f, 0},
    {0x14, -19.0f, 3, lbl_804BED10, 0, 1, 0, 1, 1, 0.0f, 60.0f, -10.0f, 0.5f, "A10503COASTER_BRK.DFF"},
    {0x19, -19.0f, 5, lbl_804BED1C, 0, 1, 0, 1, 1, -20.0f, 50.0f, 5.0f, 0.3f, "A10603COASTER_BRK.DFF"},
    {0x16, -30.0f, 3, lbl_804BED30, 0, 1, 1, 1, 1, 10.0f, 80.0f, 10.0f, 2.0f, 0},
};

/* The radius of the seat's collider. A constant with a dynamic initializer, so it lives in .sbss
 * and the static initializer stores it. */
static float lbl_805EEEBC = MetresToUnits(1.0f);

/* The current stage's record (set by the load hook). */
static const CoasterStageConfig *lbl_805EEEC0;

/* The five parameters the owner supplies (names from the parameter table). */
struct CoasterParams {
    /* 0x00 */ u32 pathNo;
    /* 0x04 */ float moveSpeed;   /* m/sec, then units */
    /* 0x08 */ float launchSpeed; /* m/sec, then units */
    /* 0x0C */ float launchAngle; /* degrees, then radians */
    /* 0x10 */ float noControlTime;
};
SIZE_ASSERT(CoasterParams, 0x14);

class Coaster : public Gadget {
  public:
    Coaster(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Coaster();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual bool HasEnded();

    /* Creates the coaster under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The collider's callback: hangs a player who touches the car (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Hangs rider in the seat (name inferred). */
    void Hang(boost::shared_ptr<CharInfo> rider);
    /* The camera mode's update handler: places the camera behind the seat (name inferred). */
    void OnCameraUpdate(CameraModeUser::UpdateEventArgs &args);
    /* Places the model, the target and the collider (name inferred). */
    void UpdateModel();
    /* Builds the car's and the seat's matrices (name inferred). */
    void UpdateMatrix();
    /* Finds the path and puts the car at its start (name inferred). */
    void InitPath();
    /* Copies the owner's parameters (name inferred). */
    void LoadParams(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x030 */ boost::shared_ptr<PathCtrl> m_path;
    /* 0x038 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x040 */ boost::shared_ptr<Target> m_target;
    /* 0x048 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x050 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x058 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x060 */ s32 m_state; /* 0 waiting, 1 riding, 2 running back, 3 waiting at the end */
    /* 0x064 */ CoasterParams m_params;
    /* 0x078 */ s32 m_backward;          /* 1 when the car runs from the end */
    /* 0x07C */ Vec3f m_rotation; /* along the path */
    /* 0x088 */ Mtx34f m_matrix;
    /* 0x0B8 */ Mtx34f m_seatMatrix;
    /* 0x0E8 */ Vec3f m_seatPosition;
    /* 0x0F4 */ Vec3f m_seatRotation;
    /* 0x100 */ float m_tilt;
    /* 0x104 */ float m_inputDelay;
    /* 0x108 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x124 */ u8 m_fading; /* the model fades in */
    /* 0x128 */ float m_fadeTime;
    /* 0x12C */ boost::shared_ptr<CameraModeUser> m_camera;
    /* 0x134 */ u32 field_0x134;
};
SIZE_ASSERT(Coaster, 0x138);

const char *Coaster::s_taskName = "Coaster";

Coaster::Coaster(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0), m_backward(0), m_rotation(0.0f, 0.0f, 0.0f),
      m_seatRotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_camera = fn_80009704(0, 0);
    fn_800A673C(&m_camera.get()->m_onUpdate,
                &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                    boost::bind(&Coaster::OnCameraUpdate, this, _1)));
    m_chara = fn_8015CD90(0x43, 4);
    m_chara->field_0x30 = 1;
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit.get()->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Coaster::OnCommand, this, _1)));
    LoadParams(owner);
    m_model = fn_8015F800(1, "COASTER.BIN");
    m_model->fn_80162A68(0, m_fading);
    fn_80161FC8(m_model.get(), 0, boost::weak_ptr<CharInfo>(m_chara));
    InitPath();
    UpdateMatrix();
    {
        boost::shared_ptr<CharaColliBase> collider(fn_800C7EA8(
            fn_80021B08(), &m_seatPosition, 0x100, 10, 0, boost::weak_ptr<CharInfo>(lbl_805EE698),
            &lbl_805E07A0, lbl_805EEEBC));
        m_handle = fn_8015B8C4(collider);
        fn_800F5980(m_handle.get(), boost::bind(&Coaster::OnColliDetect, this, _1));
    }
    m_target = fn_801E7128(boost::weak_ptr<CharInfo>(m_chara), 0x2000);
    UpdateModel();
    m_particles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_particles;
        fn_8015FA78(m_model.get(), fn_80169D00, &attach);
    }
}

Coaster::~Coaster() {
    fn_801CE9F4(fn_80099954(), boost::shared_ptr<CameraMode>(m_camera));
    m_sound.fn_801D455C();
}

extern "C" void fn_80132BC8(void) {
    fn_8006B474("COASTER.BIN", 1);
    s32 stage = lbl_8057D6A0.m_stage;
    for (int i = 0; i < 5; i++) {
        if (stage == lbl_804BED6C[i].stage) {
            lbl_805EEEC0 = &lbl_804BED6C[i];
            break;
        }
    }
}

extern "C" void fn_80132BBC(void) {
    lbl_805EEEC0 = 0;
}

void Coaster::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Coaster(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void Coaster::Behave(float dt) {
    if (m_path.get() != 0 && fn_800ABDCC(m_path.get())) {
        boost::shared_ptr<CharInfo> player(fn_800A8B74(fn_80074194(), lbl_805EE588).lock());
        if (player.get() == 0) {
            return;
        }

        if (m_fading) {
            m_fadeTime += dt;
            if (m_fadeTime > 0.6f) {
                fn_8015FD10(m_model.get());
                m_fading = 0;
            } else {
                fn_8015FDD8(m_model.get(), m_fadeTime / 0.6f);
            }
        }

        if (!fn_8000B044(m_tilt)) {
            m_tilt = TurnAngleToward(0.0f, m_tilt, DegToRad(240.0f * dt));
            m_tilt = fn_80070460(m_tilt, DegToRad(-90.0f), DegToRad(90.0f));
        }

        switch (m_state) {
        case 0:
        case 3:
            fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_80018634(m_path.get()));
            m_rotation = fn_80018434(m_path.operator->(), 1, 0);
            UpdateMatrix();
            break;
        case 1: {
            bool end = fn_8001C91C(fn_8001B494(), m_path.get(), m_backward, m_chara.get(), &m_rotation, 1,
                                   m_params.moveSpeed * dt);
            UpdateMatrix();
            if (end) {
                fn_801CE9F4(fn_80099954(), boost::shared_ptr<CameraMode>(m_camera));

                Vec3f velocity(0.0f, 0.0f, -1.0f);
                float pitch = m_params.launchAngle;
                float yaw = m_rotation.y;
                if (m_backward == 1) {
                    yaw += DegToRad(180.0f);
                }
                AssignVector(velocity, fn_8040D118(&velocity, pitch));
                AssignVector(velocity, fn_8040D0AC(&velocity, yaw));
                fn_8040CE38(&velocity, m_params.launchSpeed);
                player->PushCommand(new Player::IgnoreInputCommand(0, m_params.noControlTime));
                player->PushCommand(new ClrVelocityCommand(4));
                player->PushCommand(new SetPositionCommand(m_seatPosition, 4));
                player->PushCommand(new SetVelocityCommand(velocity, 4));
                player->PushCommand(new Player::LaunchCommand(0, 4));

                if (m_particles.get() != 0) {
                    fn_80166F88(m_particles.get());
                }
                UnkSoundEntry_80208888 *entry = fn_80208848(fn_80012700(fn_800122D4(), 1, "COASTER.BIN"), 1);
                if (entry != 0) {
                    fn_801D54F4(fn_80053164(), entry->m_soundId, m_chara.get(), 0, 1);
                }
                m_sound.fn_801D455C();

                if (!lbl_805EEEC0->breakAtEnd) {
                    if (m_backward == 0) {
                        m_state = 3;
                    } else {
                        m_state = 0;
                    }
                    m_handle->TurnOn();
                    m_handle->LetGo(0.4f);
                    if (lbl_805EEEC0->animate) {
                        m_model->fn_801604FC();
                    }
                } else {
                    m_model->fn_80161880();
                    fn_80162790(m_model.get());
                    if (lbl_805EEEC0->debrisModel != 0) {
                        UnkDebrisParam_8015F0B8 debris(boost::shared_ptr<CharInfo>(m_chara), 0);

                        debris.model = (s32)fn_80012700(fn_800122D4(), 1, lbl_805EEEC0->debrisModel);
                        debris.field_0x40 = m_chara->m_position.y;
                        debris.scale = 1.0f;
                        debris.field_0x4A = 0;
                        debris.field_0x54 = 1;
                        fn_8015D93C(&debris);
                    }
                    fn_800C9AD4(m_owner);
                    m_flags |= 1;
                    return;
                }
            } else {
                if (m_inputDelay > 0.0f) {
                    m_inputDelay -= dt;
                }
                Hang(player);
            }
            break;
        }
        case 2: {
            bool end = fn_8001C91C(fn_8001B494(), m_path.get(), m_backward, m_chara.get(), &m_rotation,
                                   lbl_805EEEC0->returnLoops, m_params.moveSpeed * dt);
            UpdateMatrix();
            if (end) {
                if (m_particles.get() != 0) {
                    fn_80166F88(m_particles.get());
                }
                UnkSoundEntry_80208888 *entry = fn_80208848(fn_80012700(fn_800122D4(), 1, "COASTER.BIN"), 1);
                if (entry != 0) {
                    fn_801D54F4(fn_80053164(), entry->m_soundId, m_chara.get(), 0, 1);
                }
                m_sound.fn_801D455C();

                if (!lbl_805EEEC0->breakAtEnd) {
                    m_handle->TurnOn();
                    if (lbl_805EEEC0->animate) {
                        m_model->fn_801604FC();
                    }
                    if (m_backward == 0) {
                        m_state = 3;
                    } else {
                        m_state = 0;
                    }
                } else {
                    m_model->fn_80161880();
                    fn_80162790(m_model.get());
                    if (lbl_805EEEC0->debrisModel != 0) {
                        UnkDebrisParam_8015F0B8 debris(boost::shared_ptr<CharInfo>(m_chara), 0);

                        debris.model = (s32)fn_80012700(fn_800122D4(), 1, lbl_805EEEC0->debrisModel);
                        debris.field_0x40 = m_chara->m_position.y;
                        debris.scale = 1.0f;
                        debris.field_0x4A = 0;
                        debris.field_0x54 = 1;
                        fn_8015D93C(&debris);
                    }
                    fn_800C9AD4(m_owner);
                    m_flags |= 1;
                    return;
                }
            }
            break;
        }
        }

        UpdateModel();
        m_model->fn_80162FB0(dt);
        if (m_particles.get() != 0) {
            fn_8016761C(m_particles.get(), dt);
        }
        if (m_handle.get() != 0) {
            fn_8015BE9C(m_handle.get(), dt);
            m_handle.get()->m_collider->SetCharInfo((CharInfo *)&m_seatPosition);
        }
    }
}

void Coaster::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    CoasterParams previous = m_params;
    u8 segment[0x18];

    LoadParams(owner);
    if (previous.pathNo != m_params.pathNo) {
        m_state = 0;
        InitPath();
        m_handle->TurnOn();
    }
    UpdateMatrix();
    UpdateModel();
    m_model.get()->fn_80162EF4();
    m_model.get()->fn_8016108C();
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }

    Vec3f direction(0.0f, 0.0f, -1.0f);
    float yaw;
    GetYaw(&yaw, m_chara->m_rotation);
    AssignVector(direction, fn_8040D118(&direction, m_params.launchAngle));
    AssignVector(direction, fn_8040D0AC(&direction, yaw));
    fn_8040CE38(&direction, 50.0f);
    fn_80020A20(segment, &m_seatPosition, &(const Vec3f &)fn_8040CDD0(&m_seatPosition, &direction));
    fn_802E0B58(segment, &lbl_805F1370);
}

bool Coaster::HasEnded() {
    if (m_owner != 0) {
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
            return true;
        }
        if (m_state == 2 || m_state == 3) {
            boost::shared_ptr<CharInfo> player(fn_800A8B74(fn_80074194(), lbl_805EE588).lock());
            if (player.get() != 0) {
                Vec3f offset;
                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         &player->m_position, &m_chara->m_position));
                float range = fn_800C9938(m_owner);
                if (fn_8040CF50(&offset) > range) {
                    m_flags |= 1;
                    return true;
                }
            }
        } else if (m_state == 0) {
            if (fn_800C996C(m_owner)) {
                m_flags |= 1;
                return true;
            }
        }
    }
    return false;
}

void Coaster::LoadParams(UnkPlacement_800C9CD0 *owner) {
    u32 flags;

    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E70(owner, &flags);
    if (flags & 0x400000) {
        m_fading = 1;
        m_fadeTime = 0.0f;
    } else {
        m_fading = 0;
        m_fadeTime = 0.0f;
    }
    if (m_params.pathNo >= lbl_805EEEC0->pathCount) {
        m_params.pathNo = lbl_805EEEC0->pathCount - 1;
        fn_800C9D68(owner, &m_params, sizeof(m_params));
    }
    m_params.moveSpeed = MetresToUnits(m_params.moveSpeed);
    m_params.launchSpeed = MetresToUnits(m_params.launchSpeed);
    m_params.launchAngle = DegToRad(m_params.launchAngle);
}

void Coaster::InitPath() {
    m_path.reset();
    m_path = boost::shared_ptr<PathCtrl>(new PathCtrl);
    if (!fn_8001C7B4(fn_8001B494(), m_path.get(), lbl_805EEEC0->paths[m_params.pathNo])) {
        m_path.reset();
    }
    m_backward = 0;
    m_tilt = 0.0f;
    if (m_path.get() != 0) {
        fn_80018304(m_path.get());
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_80018634(m_path.get()));
        m_rotation = fn_80018434(m_path.operator->(), 1, 0);
    }
    if (lbl_805EEEC0->animate) {
        m_model.get()->fn_801604FC();
    }
}

void Coaster::UpdateMatrix() {
    fn_8040C148(&m_matrix, &lbl_805E0720);
    fn_8040B80C(&m_matrix, m_rotation.z);
    fn_8040B884(&m_matrix, m_rotation.x);
    fn_8040B848(&m_matrix, m_rotation.y);
    if (lbl_805EEEC0->levelCamera) {
        Vec3f angles(0.0f, 0.0f, 0.0f);
        fn_800494F8(&angles, &m_matrix);
        fn_8040C148(&m_matrix, &lbl_805E0720);
        fn_8040B848(&m_matrix, angles.y);
    }
    fn_8040C148(&m_seatMatrix, &m_matrix);
    if (m_backward == 1) {
        fn_8040B848(&m_seatMatrix, 180.0f * (lbl_805FAD38 / 180.0f));
    }

    if (!fn_8000B044(m_tilt)) {
        Vec3f axis(0.0f, 0.0f, 1.0f);
        AssignVector(axis, fn_8040B67C(&m_matrix, &axis));
        Mtx34f tilt;
        fn_800498A0(&tilt, &axis, m_tilt);
        fn_8040C148(&m_matrix, &(const Mtx34f &)fn_8001E044(&tilt, &m_matrix));
        fn_8040C148(&m_seatMatrix, &(const Mtx34f &)fn_8001E044(&tilt, &m_seatMatrix));
    }

    fn_800493C4(&m_chara->m_rotation, &m_matrix);
    Mtx34f translation;
    fn_8040C148(&translation, &lbl_805E0720);
    fn_8040B59C(&translation, &m_chara->m_position);
    fn_8040C148(&m_matrix, &(const Mtx34f &)fn_8001E044(&translation, &m_matrix));
    fn_8040C148(&m_seatMatrix, &(const Mtx34f &)fn_8001E044(&translation, &m_seatMatrix));

    Vec3f seat(0.0f, lbl_805EEEC0->seatHeight, 0.0f);
    AssignVector(m_seatPosition, fn_8040B67C(&m_matrix, &seat));
}

void Coaster::UpdateModel() {
    m_model->fn_801633DC(&m_matrix);
    m_target.get()->fn_800CCE38(&m_chara->m_position);
    if (m_handle.get() != 0) {
        m_handle.get()->m_collider->SetCharInfo((CharInfo *)&m_seatPosition);
    }
}

void Coaster::OnCameraUpdate(CameraModeUser::UpdateEventArgs &args) {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B80C(&matrix, m_rotation.z);
    fn_8040B884(&matrix, m_rotation.x);
    fn_8040B848(&matrix, m_rotation.y);
    if (lbl_805EEEC0->levelCamera) {
        Vec3f angles(0.0f, 0.0f, 0.0f);
        fn_800494F8(&angles, &matrix);
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B848(&matrix, angles.y);
    }
    if (m_backward == 1) {
        fn_8040B848(&matrix, 180.0f * (lbl_805FAD38 / 180.0f));
    }

    Vec3f eye(0.0f, lbl_805EEEC0->seatHeight, 0.0f);
    fn_8040CFF8(&eye, &(const Vec3f &)((VectorCtorFn)fn_80008E1C)(
                          0.0f, lbl_805EEEC0->cameraSide, lbl_805EEEC0->cameraBack));
    fn_800091D0(&eye, &(const Vec3f &)fn_8040CDD0(
                          &(const Vec3f &)fn_8040B67C(&matrix, &eye), &m_chara->m_position));
    Vec3f target(0.0f, lbl_805EEEC0->seatHeight, 0.0f);
    target.y += lbl_805EEEC0->lookHeight;
    fn_800091D0(&target, &(const Vec3f &)fn_8040CDD0(
                             &(const Vec3f &)fn_8040B67C(&matrix, &target), &m_chara->m_position));
    fn_8000A5D4(m_camera.get(), &eye, &target);
    Vec3f up(0.0f, 1.0f, 0.0f);
    AssignVector(up, fn_8040B67C(&matrix, &up));
    fn_8000ACE4(m_camera.get(), &up);
}

void Coaster::Hang(boost::shared_ptr<CharInfo> rider) {
    fn_800494F8(&m_seatRotation, &m_seatMatrix);
    rider->PushCommand(new Player::TouchHangCommand(boost::weak_ptr<CharInfo>(m_chara), &m_seatPosition,
                                                    &m_seatRotation, 3, 4));
}

void Coaster::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 0 || m_state == 3) {
        if (fn_8015D2EC(args.m_contacts)) {
            boost::shared_ptr<CharInfo> player(fn_800A8B74(fn_80074194(), lbl_805EE588).lock());
            if (player.get() != 0) {
                Hang(player);
            }
        }
    }
}

void Coaster::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB: /* RideCommand */
        if (m_state == 0 || m_state == 3) {
            m_handle->TurnOff();
            if (lbl_805EEEC0->animate) {
                m_model.get()->fn_80160C28(0);
                fn_8016082C(m_model.get(), 0);
                m_model.get()->fn_80160B18(0);
            }
            if (m_state == 0) {
                m_backward = 0;
                if (m_particles.get() != 0) {
                    fn_80167010(m_particles.get(), 1);
                }
            } else {
                m_backward = 1;
                if (m_particles.get() != 0) {
                    fn_80167010(m_particles.get(), 2);
                }
            }
            m_inputDelay = 0.4f;
            {
                UnkCameraBlend_8000B2C0 blend(lbl_805EEEC0->blendTime);
                fn_801CEA54(fn_80099954(), boost::shared_ptr<CameraMode>(m_camera), 2, blend, 0, 1);
            }

            UnkModelTable_80208888 *table = fn_80012700(fn_800122D4(), 1, "COASTER.BIN");
            UnkSoundEntry_80208888 *entry = fn_80208848(table, 0);
            if (entry != 0) {
                fn_801D54F4(fn_80053164(), entry->m_soundId, m_chara.get(), 0, 1);
            }
            entry = fn_80208848(table, 2);
            if (entry != 0) {
                m_sound = fn_801D3DC8((u16)entry->m_soundId, m_chara.get(), this);
            }
            m_state = 1;
        }
        break;
    case 0xC: /* LeaveCommand */
        fn_801CE9F4(fn_80099954(), boost::shared_ptr<CameraMode>(m_camera));
        m_state = 2;
        break;
    case 1: /* AccelerateInputCommand */
        if (lbl_805EEEC0->tiltInput && m_inputDelay <= 0.0f) {
            AccelerateInputCommand *input = static_cast<AccelerateInputCommand *>(&command);
            if (input != 0) {
                float speed = 540.0f;
                speed *= lbl_8057D6A0.field_0x28;
                speed *= fn_8040CF50(&input->m_direction);

                Mtx34f matrix;
                fn_8040C148(&matrix, &lbl_805E0720);
                fn_8040B80C(&matrix, m_rotation.z);
                fn_8040B884(&matrix, m_rotation.x);
                fn_8040B848(&matrix, m_rotation.y);
                if (lbl_805EEEC0->levelCamera) {
                    Vec3f angles(0.0f, 0.0f, 0.0f);
                    fn_800494F8(&angles, &matrix);
                    fn_8040C148(&matrix, &lbl_805E0720);
                    fn_8040B848(&matrix, angles.y);
                }
                fn_8040BCD4(&matrix);

                Vec3f local;
                ConstructVector(&local, fn_8040B67C(&matrix, &input->m_direction));
                local.y = 0.0f;
                fn_8040CE74(&local);
                float side = 0.0f;
                float limit = std::cos(60.0f * (lbl_805FAD38 / 180.0f));
                if (std::fabs(local.x) > limit) {
                    side = local.x;
                }
                m_tilt += DegToRad(speed * side);
            }
        }
        break;
    }
    command.Flags() |= 2;
}

static GadgetParamDesc lbl_805288BC[] = {
    {2, "PATH_NO", 0, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "MOVE_SPPED m/sec", 0, 0, 0, 50.0f, 0.01f, 65536.0f, fn_800CBC20, 0},
    {4, "LAUNCH_SPD m/sec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "LAUNCH_ANG deg", 0, 0, 0, 0.0f, -90.0f, 90.0f, fn_800CBC20, 0},
    {4, "LAUNCH_NO_CTRL_TIME sec", 0, 0, 0, 0.0f, 0.0f, 100.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805289AC = {
    "Coaster", fn_80132BC8, fn_80132BBC, Coaster::Instantiate, 0, 0, 0xC88, 1, 0, 10, lbl_805288BC,
};
