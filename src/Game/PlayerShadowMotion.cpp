/* Player::Shadow::ShadowMotion (class name from RTTI; bases Task and Player::PlayerMotion): the
 * motion of Shadow. Once the player's model resource is ready it loads the motion, the model,
 * the hands (Player::Shadow::MotionHand), the eyes (Player::Shadow::MotionEye) and the extra
 * models (Player::Shadow::MotionSub), hooks its callbacks on the skeleton's nodes (the head, the
 * held object, the arms and the inverse-kinematics slots), then each frame places the motion on
 * the player and moves the player by the motion's root. Each motion of a table brings two step
 * callbacks that pick the motion to play. The file name comes from the class; function, member
 * and helper names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#define PLAYER_MOTION_MEMBERS
#define PLAYER_MOTION_HAND_MATRIX
#define PLAYER_MOTION_SLOT_ARGS
#define PLAYER_OBJECT_MEMBERS
#define PLAYER_PICKUP_SLOT
#define PLAYER_TARGET_BOOL_SLOT_10
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerShadowMotionEye.h"
#include "Game/PlayerShadowMotionFiles.h"
#include "Game/PlayerShadowMotionSub.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Quaternion.h"
#include "Game/Task.h"
#include "Game/UnkMatrix_800B1304.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;

struct RpClump;
struct _stMtnPlayWork;
class PickupCharInfo;

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls (see Game/PlayerMotionEvent.h). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

extern "C" {
/* PlayerMotion's own functions (see Game/NpcSonicMotion.cpp; behaviour inferred). */
bool fn_800A9E1C(const boost::shared_ptr<PlayerBase> &player);
void fn_800A9E48(Player::PlayerMotion *motion);
void fn_800A9EA0(Player::PlayerMotion *motion);
float fn_800A9EF8(Player::PlayerMotion *motion);
void fn_800A9F9C(Player::PlayerMotion *motion, float frame);
float fn_800AA064(Player::PlayerMotion *motion);
void fn_800AA0D4(Player::PlayerMotion *motion);
void fn_800AA404(Player::PlayerMotion *motion);
UnkClumpFade_80027D5C *fn_800AA584(Player::PlayerMotion *motion);
int fn_80093C60(const boost::shared_ptr<PlayerBase> *player, const Vec3f *direction);

/* The resource loader and its files (see Game/PlayerShadowMotionHand.cpp). */
void *fn_800122D4(void);
s32 fn_80012408(void *loader, s32 model);
void *fn_80012684(void *loader, s32 file, s32 index);
s32 fn_80012608(void *loader, s32 file, s32 index);
RpClump *fn_8001277C(void *loader, s32 file, s32 index);
s32 fn_80078C4C(PlayerBase *player);

/* Motions (see Game/PlayerShadowMotionHand.cpp). */
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80415EA4(_stMtnPlayWork *work);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80415E34(_stMtnPlayWork *work, Vec3f *position);
void fn_80415BDC(_stMtnPlayWork *work, const Vec3f *delta);
void fn_8041734C(_stMtnPlayWork *work, const void *node);
/* Hooks a callback on a node of a motion; the callback gets the node and data (behaviour
 * inferred). */
void fn_80415808(_stMtnPlayWork *work, int node, void (*callback)(void *), void *data);

/* Clumps (see Game/PlayerShadowMotionHand.cpp). */
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void fn_8006CAE4(Player::UnkClumpList_8006CAE4 *list, RpClump *const *clump);

/* Flag words and the player's state bits. */
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(const u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);

/* The player: its front and up, an update, its shadow, and a point in its frame (behaviour
 * inferred). */
Vec3f fn_80079698(PlayerBase *player);
Vec3f fn_800796DC(PlayerBase *player);
void fn_80079860(PlayerBase *player);
void fn_803563D0(void *shadow, float arg1);
Vec3f fn_80079628(PlayerBase *player, const Vec3f *offset);
/* The weapon type's reach (behaviour inferred). */
float fn_8005B904(int type);
/* Places a carried object (see Game/PlayerBehaviorPickupObject.cpp). */
void fn_801E41C4(PickupCharInfo *object, const Mtx34f *matrix);

/* A node's matrix, read and written (behaviour inferred). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
/* Two-bone inverse kinematics of the arms, and the head's turn (behaviour inferred). */
void fn_80015DBC(Mtx34f *upper, Mtx34f *lower, Mtx34f *hand, const Vec3f *target, const Mtx34f *pole,
                 float arg1, float arg2, float arg3);
void fn_80015B60(Mtx34f *upper, Mtx34f *lower, Mtx34f *hand, const Vec3f *target, const Mtx34f *pole,
                 float arg1, float arg2, float arg3);
void fn_80015DE0(Mtx34f *upper, Mtx34f *lower, float arg1, float arg2, const Vec3f *target, void *node);
/* Arc cosine; a clamp; a random value in [0, 1) (see Game/PlayerTargetBase.cpp). */
float fn_80015D68(float value);
const float &fn_80070460(const float &value, const float &low, const float &high);
float fn_8040EE90(void);

/* Vectors. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *v);
float fn_8040CE74(Vec3f *v);
void fn_8040CE60(Vec3f *v);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
Vec3f fn_80016284(const Vec3f *v);
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
Vec2f fn_8040F760(const Vec3f *direction);
void fn_8040E988(Vec2f *angles);

/* Matrices and quaternions. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040FB08(const Mtx34f *matrix);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Mtx34f fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t);
Mtx34f fn_80049778(const Vec3f *axis, float angle);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
Mtx34f fn_8040B228(const Quatf *rotation);
void fn_8040B8C0(Mtx34f *matrix, const Quatf *rotation);
Quatf fn_8040C55C(const Mtx34f *matrix);
Quatf fn_8040C3EC(const Quatf *from, const Quatf *to, float t);
void fn_8040C908(Quatf *q);
void fn_8007A34C(Quatf *rotation, const Quatf *value);
}

namespace std {
float fabs(float x);
}

/* fn_8040CD74 and fn_8040B034 called as returning their result by value. */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
typedef Mtx34f (*TranslationFn)(const Vec3f *offset);

/* Passes a returned value on by address (as in Game/NpcSonicMotion.cpp). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

inline const Quatf &RefQ(const Quatf &q) {
    return q;
}

/* fn_8040B228 called with the matrix to fill. */
typedef void (*QuatToMatrixFn)(Mtx34f *matrix, const Quatf *rotation);

extern const float lbl_805FAD38; /* pi */

/* A node of a motion as fn_800163F4 fills it (struct name unknown; see
 * Game/PlayerShadowMotionHand.cpp). */
struct UnkMotionNodeMtx_800163F4 {
    /* 0x00 */ u8 field_0x0[0x30];
};

/* The fields of a player the motion reads (names inferred). */
struct UnkShadowFields_800B10CC {
    const Mtx34f *Matrix() const {
        return &m_matrix;
    }

    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_front;
    /* 0x024 */ u8 field_0x24[0xB4 - 0x24];
    /* 0x0B4 */ u32 m_motionFlags;
    /* 0x0B8 */ u8 field_0xB8[0xC4 - 0xB8];
    /* 0x0C4 */ Vec3f m_velocity;
    /* 0x0D0 */ u8 field_0xD0[0xF4 - 0xD0];
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x198 - 0x100];
    /* 0x198 */ Mtx34f m_matrix;
    /* 0x1C8 */ Vec3f m_normal;
};

/* The player's shadow (struct name unknown): whether it is on, and its height. */
struct UnkShadow_800B3538 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_height;
    /* 0x24 */ u8 m_on;
};

#define Fields(player) reinterpret_cast<UnkShadowFields_800B10CC *>(player)

inline UnkShadow_800B3538 *ShadowOf(PlayerBase *player) {
    return *reinterpret_cast<UnkShadow_800B3538 **>(reinterpret_cast<u8 *>(player) + 0x278);
}

/* The player's matrix and kind (names inferred). */
/* Matrix copy and product through inline wrappers (names inferred): an operator-> in their
 * arguments is called out of line, as retail does. */
inline void CopyMatrix(Mtx34f &to, const Mtx34f &from) {
    fn_8040C148(&to, &from);
}

inline void MultiplyMatrix(Mtx34f &to, const Mtx34f &by) {
    fn_8040BAC4(&to, &by);
}

inline s32 KindOf(PlayerBase *player) {
    return player->m_kind;
}

/* The stage (class name unknown; see Game/BulletManagerTask.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

class UnkStage_8003A840 {
  public:
    UnkWorldEntry_8051D208 *GetAreas() {
        return lbl_8051D208;
    }
    void *GetWorld(int area) {
        return GetAreas()[area].m_world;
    }
};

extern "C" UnkStage_8003A840 *fn_8003A840(void);

/* Puts clump in world (name inferred; as in Game/NpcMotion.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* What MotionHand's SetMotion gets (see Game/PlayerShadowMotionHand.cpp). */
struct UnkHandMotion_800B07FC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_left;
    /* 0x8 */ s32 m_right;
};

inline const Vec3f &RefAim(const Player::AimPosition &position) {
    return reinterpret_cast<const Vec3f &>(position);
}

namespace Player {
namespace Shadow {

class MotionHandImpl;

/* MotionHand (Game/PlayerShadowMotionHand.cpp). */
class MotionHand : public Task {
  public:
    MotionHand(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files);
    virtual ~MotionHand();
    virtual void Tick(float dt);

    void Hide();
    void Show();
    void GetAnims(_stMtnPlayWork **left, _stMtnPlayWork **right);
    Mtx34f *GetNodeMatrix(int node);
    void SetTime(float time);
    void SetMotion(const UnkHandMotion_800B07FC *motion, float blendTime);
    void SetPose(int pose, float blend);
    void SetSlot(int slot);
    void AddTo(UnkClumpList_8006CAE4 *list);
    void Update();

    /* 0x28 */ boost::scoped_ptr<MotionHandImpl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};

class ShadowMotion;

/* What a node callback gets (struct name unknown): the node's frame and the motion. */
struct UnkNodeArgs_800B10CC {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ ShadowMotion *m_motion;
};

/* The callback of an inverse-kinematics slot (class name unknown; as
 * UnkMotionEventCallback_8007B470 in Game/PlayerMotionEvent.h). */
class UnkIKCallback_800B4D30 {
  public:
    UnkIKCallback_800B4D30 &operator=(const IKCallback &f) {
        m_function = f;
        return *this;
    }
    UnkIKCallback_800B4D30 &Clear() {
        m_function.clear();
        return *this;
    }
    bool HasFunction() const {
        return !m_function.empty();
    }
    bool IsSet() const {
        return HasFunction();
    }
    void Call(IKEventArgs &args) {
        if (!m_function.empty()) {
            m_function(args);
        }
    }

    /* 0x0 */ IKCallback m_function;
};

/* An inverse-kinematics slot (struct name unknown): the arm's three bones, the matrix the hand is
 * held at, the callback that places the target, whether the matrix or the callback is used, and
 * the weight. */
struct UnkIKSlot_800B1264 {
    UnkIKSlot_800B1264() : m_active(0), m_weight(0.0f) {}

    /* 0x00 */ UnkMatrix_800B1304 m_bones[3];
    /* 0x90 */ Mtx34f m_matrix;
    /* 0xC0 */ UnkIKCallback_800B4D30 m_callback;
    /* 0xCC */ u8 m_active;
    /* 0xD0 */ float m_weight;
};

/* The head's rotation (struct name unknown). */
struct UnkHeadTurn_800B1308 {
    UnkHeadTurn_800B1308() {
        SetIdentity();
    }
    void SetIdentity() {
        fn_8040C908(&m_rotation);
    }
    void Update(UnkNodeArgs_800B10CC *args, float dt);

    /* 0x0 */ Quatf m_rotation;
};

/* What a motion runs before and after the motion is placed each frame (struct name unknown). */
typedef void (ShadowMotion::*StepFunc)();

struct MotionSteps {
    StepFunc m_before;
    StepFunc m_after;
};

/* A motion of the table (struct name unknown): its index in the model's motions (-1: none), the
 * index it switches to once the player allows (-1: none), the hands' pose and its steps. */
struct MotionEntry {
    /* 0x0 */ s32 m_index;
    /* 0x4 */ s32 m_nextIndex;
    /* 0x8 */ s32 m_handPose;
    /* 0xC */ const MotionSteps *m_steps;
};

class ShadowMotion : public PlayerMotion {
  public:
    ShadowMotion(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 *files);
    virtual ~ShadowMotion();
    virtual void Tick(float dt);
    virtual void vf_0x10(int motion, float blendTime);
    virtual void vf_0x14(UnkClumpList_8006CAE4 *list);
    virtual void SetWeaponPose(int pose);
    virtual void vf_0x1C(const UnkHandMotion_800B07FC *motion, float blendTime);
    virtual void vf_0x20(int slot);
    virtual void vf_0x24(int slot, const Mtx34f *matrix);
    virtual void SetIKCallback(int slot, const IKCallback &callback);
    virtual void vf_0x2C(int slot, float weight);
    virtual float vf_0x30(int slot);
    virtual void vf_0x34(_stMtnPlayWork **left, _stMtnPlayWork **right);
    virtual int vf_0x38();
    virtual void Show();
    virtual void Hide();

    /* The run's speed and how far it is into its speed's motion (the words at 0x438 and 0x448). */
    float &RunSpeed() {
        return *reinterpret_cast<float *>(&field_0x438);
    }
    float &RunBlend() {
        return *reinterpret_cast<float *>(&field_0x444[4]);
    }

    void LoadWhenReady();
    void Run(float dt);
    void UpdateVisibility();
    void LoadMotion();
    void Unload();
    void BindNodes();
    void OnIK(IKEventArgs &args);
    void InstallIK();

    void StartMotion();
    void WaitForPlayer();
    void Stand();
    void SelectRunMotion();
    void Walk();
    void StartWalk();
    void Fall();
    void Land();
    void TurnToPlayer();
    void Step800B334C();
    void Step800B32B4();
    void Step800B31E0();
    void Step800B30F8();
    void Step800B2F48();
    void Step800B2EA4();
    void Step800B2E28();
    void Step800B2D38();
    void Idle();
    void Step800B2C0C();
    void Step800B2B48();
    void Step800B2B44();
    void WaitForLanding();
    void Step800B2A18();
    void Step800B2844();
    void Step800B2814();
    void Step800B27E4();

    /* 0x458 */ s32 m_state; /* 0: waiting for the model resource, 1: running */
    /* 0x45C */ UnkClumpUpdater_80015A7C *m_part;
    /* 0x460 */ StepFunc m_before;
    /* 0x46C */ StepFunc m_after;
    /* 0x478 */ float m_dt;
    /* 0x47C */ UnkHeadTurn_800B1308 m_head;
    /* 0x48C */ u8 m_moved;
    /* 0x490 */ UnkIKSlot_800B1264 m_ik[4];
    /* 0x7E0 */ MotionHand *m_hand;
    /* 0x7E4 */ MotionEye *m_eye;
    /* 0x7E8 */ MotionSub *m_sub;
    /* 0x7EC */ UnkMotionFiles_800ADE78 m_files;
    /* 0x864 */ u32 pad_0x864; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ShadowMotion, 0x868);

/* The height below which the player stops falling (name inferred) and the point the aim callback
 * starts from. */
inline float DynamicFloat(float value) {
    return value;
}

static const float s_landHeight = DynamicFloat(40.0f);
static const Vec3f s_aimOffset(1.4375f, 3.0f, 0.28125f);

/* The motions' steps. */
static MotionSteps s_default = {&ShadowMotion::StartMotion, &ShadowMotion::WaitForPlayer};
static MotionSteps s_3AAC = {&ShadowMotion::Stand, 0};
static MotionSteps s_38C8 = {&ShadowMotion::SelectRunMotion, 0};
static MotionSteps s_36F8 = {&ShadowMotion::Walk, 0};
static MotionSteps s_3624 = {&ShadowMotion::StartWalk, 0};
static MotionSteps s_3538 = {&ShadowMotion::Fall, 0};
static MotionSteps s_34E0 = {&ShadowMotion::Land, 0};
static MotionSteps s_33D8 = {&ShadowMotion::TurnToPlayer, 0};
static MotionSteps s_334C = {&ShadowMotion::Step800B334C, &ShadowMotion::Step800B32B4};
static MotionSteps s_31E0 = {&ShadowMotion::Step800B31E0, 0};
static MotionSteps s_30F8 = {&ShadowMotion::Step800B30F8, 0};
static MotionSteps s_2F48 = {&ShadowMotion::Step800B2F48, 0};
static MotionSteps s_2EA4 = {&ShadowMotion::Step800B2EA4, 0};
static MotionSteps s_2E28 = {&ShadowMotion::Step800B2E28, 0};
static MotionSteps s_2D38 = {&ShadowMotion::Step800B2D38, 0};
static MotionSteps s_2C70 = {&ShadowMotion::Idle, 0};
static MotionSteps s_2C0C = {&ShadowMotion::Step800B2C0C, 0};
static MotionSteps s_2B48 = {&ShadowMotion::Step800B2B48, 0};
static MotionSteps s_2A84 = {&ShadowMotion::Step800B2B44, &ShadowMotion::WaitForLanding};
static MotionSteps s_2A18 = {&ShadowMotion::Step800B2A18, 0};
static MotionSteps s_2844 = {&ShadowMotion::Step800B2844, 0};
static MotionSteps s_2814 = {&ShadowMotion::Step800B2814, 0};
static MotionSteps s_27E4 = {&ShadowMotion::Step800B27E4, 0};
static MotionSteps s_none = {0, 0};

/* The skeleton's nodes by character kind (-1: the model's matrix, -2: the character's). */
static const int s_nodes[3][14] = {
    {-1, -2, 0x1E, 0x18, 0x35, 0x1C, 0x49, 0x2C, 0x2B, 0x30, 0x2F, 0x28, -1, -1},
    {-1, -2, 0x1E, 0x18, 0x18, 0x1C, 0x1C, 0x2C, 0x2B, 0x30, 0x2F, 0x28, 0x2329, 0x2332},
    {-1, -2, 0x1E, 0x18, 0x18, 0x1C, 0x1C, 0x2C, 0x2B, 0x30, 0x2F, 0x28, 0x2329, -1},
};

static const MotionEntry s_motions[48] = {
    {-1, -1, 0x2, &s_3AAC},
    {-1, -1, 0x1, &s_38C8},
    {-1, -1, 0x1, &s_36F8},
    {-1, -1, 0x0, &s_3538},
    {-1, -1, 0x0, &s_34E0},
    {-1, -1, 0x0, &s_3624},
    {0x53, -1, 0x0, &s_default},
    {-1, -1, 0x0, &s_33D8},
    {0x2A, -1, 0x0, &s_default},
    {0x3C, -1, 0x0, &s_default},
    {0x3B, -1, 0x0, &s_default},
    {0x49, -1, 0x0, &s_default},
    {-1, -1, 0x1, &s_334C},
    {0x2C, 0x37, 0x6, &s_default},
    {-1, -1, 0x6, &s_31E0},
    {-1, -1, 0x8, &s_30F8},
    {0x98, 0x53, 0x0, &s_default},
    {0x9C, 0x9B, 0x0, &s_default},
    {0x9A, 0x99, 0x0, &s_default},
    {0x9E, -1, 0x0, &s_default},
    {0x9D, -1, 0x0, &s_default},
    {-1, -1, 0x0, &s_2F48},
    {-1, -1, 0x0, &s_2EA4},
    {0x65, -1, 0x0, &s_default},
    {-1, -1, 0x0, &s_2E28},
    {0x84, -1, 0x0, &s_default},
    {0x42, -1, 0x0, &s_default},
    {0x41, -1, 0x0, &s_default},
    {-1, -1, 0xA, &s_2D38},
    {-1, -1, 0x0, &s_2C70},
    {0x29, -1, 0x4, &s_default},
    {0x2B, -1, 0x0, &s_default},
    {-1, -1, 0x0, &s_2C0C},
    {0x73, -1, 0x0, &s_default},
    {0x74, -1, 0x0, &s_default},
    {0x78, -1, 0x0, &s_2B48},
    {0x26, -1, 0xB, &s_default},
    {0x27, 0x28, 0xC, &s_default},
    {0x53, -1, 0x0, &s_default},
    {0x51, -1, 0x0, &s_default},
    {0x39, -1, 0x6, &s_default},
    {0x43, -1, 0x0, &s_2A84},
    {-1, -1, 0x0, &s_2A18},
    {-1, -1, 0x0, &s_2844},
    {-1, -1, 0x0, &s_2814},
    {-1, -1, 0x0, &s_27E4},
    {-1, -1, 0x0, &s_none},
    {-1, -1, 0x0, &s_none},
};

/* The run's motions by speed, without and with a weapon: the motion and the one blended with it. */
static const int s_runMotions[2][4][2] = {
    {{0x67, 0x93}, {0x93, 0x88}, {0x88, 0x91}, {0x91, 0x66}},
    {{0x85, 0x87}, {0x87, 0x88}, {0x88, 0x88}, {0x88, 0x88}},
};

/* The idle motions picked at random. */
static const int s_idleMotions[2] = {0x7F, 0x80};

ShadowMotion::ShadowMotion(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 *files)
    : PlayerMotion(parent, player), m_state(0), m_files(*files) {
    m_part = 0;
    m_before = 0;
    m_after = 0;
    m_hand = 0;
    m_eye = 0;
    m_sub = 0;
}

ShadowMotion::~ShadowMotion() {
    Unload();
}

void ShadowMotion::Tick(float dt) {
    m_dt = dt;
    UpdateVisibility();
    PlayerMotion::Tick(m_dt);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E) && fn_8006CD78(m_player.get()->m_stateBits, 0x3D)) {
        InstallIK();
    } else {
        vf_0x20(0);
    }
    switch (m_state) {
    case 0:
        LoadWhenReady();
        break;
    case 1:
        Run(m_dt);
        break;
    }
}

void ShadowMotion::vf_0x10(int motion, float blendTime) {
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x26) && m_motionId != motion) {
        m_prevMotionId = m_motionId;
        m_motionId = motion;
        m_blendTime = blendTime;
        m_speed = 1.0f;
        const MotionEntry *entry = &s_motions[m_motionId];
        if (entry->m_index >= 0) {
            m_nextMotion = m_motionBase - 1 + entry->m_index;
            m_nextMotionArg = -1;
        }
        m_before = entry->m_steps->m_before;
        m_after = entry->m_steps->m_after;
        m_step = 0;
        fn_80014E10(&Fields(m_player.get())->m_motionFlags);
        if (m_hand) {
            m_hand->SetPose(entry->m_handPose, m_blendTime);
        }
    }
}

void ShadowMotion::vf_0x14(UnkClumpList_8006CAE4 *list) {
    if (fn_80014E34(&m_flags.m_bits, 2)) {
        RpClump *clump = *reinterpret_cast<RpClump **>(m_part);
        fn_8006CAE4(list, &clump);
        m_hand->AddTo(list);
        m_eye->AddTo(list);
        if (m_sub) {
            m_sub->AddClumps(list);
        }
    }
}

void ShadowMotion::Show() {
    if (fn_80014E34(&m_flags.m_bits, 3)) {
        fn_8006127C(&m_flags.m_bits, 3);
        if (m_part) {
            fn_80038F94(*reinterpret_cast<RpClump **>(m_part));
        }
        if (m_hand) {
            m_hand->Show();
        }
        if (m_eye) {
            m_eye->Show();
        }
        if (m_sub) {
            m_sub->Show();
        }
        if (m_player.get()->m_weapon) {
            m_player.get()->m_weapon->ShowWeapon();
        }
    }
}

void ShadowMotion::Hide() {
    if (!fn_80014E34(&m_flags.m_bits, 3)) {
        fn_80014E1C(&m_flags.m_bits, 3);
        if (m_part) {
            fn_80038FC0(*reinterpret_cast<RpClump **>(m_part));
        }
        if (m_hand) {
            m_hand->Hide();
        }
        if (m_eye) {
            m_eye->Hide();
        }
        if (m_sub) {
            m_sub->Hide();
        }
        if (m_player.get()->m_weapon) {
            m_player.get()->m_weapon->HideWeapon();
        }
    }
}

void ShadowMotion::SetWeaponPose(int pose) {
    if (m_hand) {
        m_hand->SetSlot(pose);
    }
}

void ShadowMotion::vf_0x1C(const UnkHandMotion_800B07FC *motion, float blendTime) {
    vf_0x10(0x2F, blendTime);
    m_blendTime = blendTime;
    m_nextMotion = motion->field_0x0;
    if (m_hand) {
        m_hand->SetMotion(motion, blendTime);
    }
}

void ShadowMotion::vf_0x20(int slot) {
    if (m_ik[slot].m_active) {
        m_ik[slot].m_active = 0;
        m_ik[slot].m_callback.Clear();
        m_ik[slot].m_weight = 0.0f;
    }
}

void ShadowMotion::vf_0x24(int slot, const Mtx34f *matrix) {
    m_ik[slot].m_active = 1;
    fn_8040C148(&m_ik[slot].m_matrix, matrix);
    m_ik[slot].m_callback.Clear();
    m_ik[slot].m_weight = 1.0f;
}

void ShadowMotion::SetIKCallback(int slot, const IKCallback &callback) {
    m_ik[slot].m_active = 1;
    m_ik[slot].m_callback = callback;
    m_ik[slot].m_weight = 1.0f;
}

void ShadowMotion::vf_0x2C(int slot, float weight) {
    m_ik[slot].m_weight = weight;
}

float ShadowMotion::vf_0x30(int slot) {
    return m_ik[slot].m_weight;
}

void ShadowMotion::vf_0x34(_stMtnPlayWork **left, _stMtnPlayWork **right) {
    if (m_hand) {
        m_hand->GetAnims(left, right);
    }
}

int ShadowMotion::vf_0x38() {
    if (m_eye) {
        return m_eye->GetBlinkFrame();
    }
    return 0;
}

void ShadowMotion::LoadWhenReady() {
    void *loader = fn_800122D4();
    s32 model = fn_80078C4C(m_player.get());
    if (fn_80012408(loader, model) == 3) {
        LoadMotion();
        m_hand = new MotionHand(this, m_player, m_files);
        m_eye = new MotionEye(this, m_player, m_files);
        m_sub = MotionSub::Create(this, m_player);
        BindNodes();
        m_state = 1;
        fn_80076CA8(m_player.get()->m_stateBits, 0x47);
        fn_80076CA8(m_player.get()->m_stateBits, 0x46);
        int motion = m_motionId;
        if (motion != 0x30) {
            m_motionId = 0x30;
            vf_0x10(motion, 0.0f);
        }
    }
}

void ShadowMotion::Run(float dt) {
    fn_8006127C(&m_flags.m_bits, 2);
    if (m_work) {
        CopyMatrix(m_baseMatrix, Fields(m_player.operator->())->m_matrix);
        fn_8040B848(&m_baseMatrix, Fields(m_player.get())->m_rotation.y);
        fn_8040B884(&m_baseMatrix, Fields(m_player.get())->m_rotation.x);
        fn_8040B80C(&m_baseMatrix, Fields(m_player.get())->m_rotation.z);
        fn_8040C148(&m_bodyMatrix, &m_baseMatrix);
        Vec3f offset;
        fn_800091D0(&offset, &Ref(fn_8040B67C(&m_baseMatrix, &Vec3f(0.0f, -5.0f, 0.0f))));
        m_moved = 1;
        if (m_before) {
            (this->*m_before)();
        }
        Vec3f root;
        fn_80415E34(m_work, &root);
        fn_800091D0(&root, &Ref(((VectorSubtractFn)fn_8040CD74)(
                               &Ref(fn_8040CDD0(&Fields(m_player.get())->m_position, &offset)), &root)));
        fn_80415BDC(m_work, &root);
        UnkMotionNodeMtx_800163F4 node;
        fn_800163F4(&node, &m_baseMatrix);
        fn_8041734C(m_work, &node);
        Vec3f top;
        fn_800091D0(&top, &Ref(fn_8040B67C(&m_bodyMatrix, &Vec3f(0.0f, 1.1999999f, -1.5999999f))));
        fn_8040FA20(&m_bodyMatrix, &Ref(fn_8040CDD0(&Fields(m_player.get())->m_position, &top)));
        fn_8040FA20(&m_baseMatrix, &Fields(m_player.get())->m_position);
        fn_800AA404(this);
        if (m_startFrame >= 0.0f) {
            fn_80416E20(m_work, 0, m_startFrame);
            m_hand->SetTime(m_startFrame);
            m_startFrame = -1.0f;
            m_frameTime = 0.0f;
        } else {
            m_frameTime = fn_800A9EF8(this);
        }
        fn_80416DC8(m_work, 0, m_frameTime);
        fn_80415EA4(m_work);
        if (m_part) {
            fn_800159AC(m_part);
        }
        fn_800A9EA0(this);
        fn_80014E1C(&m_flags.m_bits, 2);
        if (m_moved) {
            Vec3f moved;
            fn_80415E34(m_work, &moved);
            UnkShadowFields_800B10CC *player = Fields(m_player.get());
            fn_800091D0(&Fields(m_player.get())->m_velocity,
                        &Ref(((VectorSubtractFn)fn_8040CD74)(
                            &Ref(((VectorSubtractFn)fn_8040CD74)(&moved, &offset)), &player->m_position)));
            fn_800091D0(&Fields(m_player.get())->m_position,
                        &Ref(((VectorSubtractFn)fn_8040CD74)(&moved, &offset)));
        } else {
            fn_8040CE60(&Fields(m_player.get())->m_velocity);
        }
        fn_800AA0D4(this);
        if (m_after) {
            (this->*m_after)();
        }
    }
    m_hand->Update();
    m_eye->Update();
    if (m_sub) {
        m_sub->Refresh();
    }
    fn_800A9E48(this);
    fn_80079860(m_player.get());
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x27) && !fn_8006CD78(m_player.get()->m_stateBits, 0x28)) {
        fn_803563D0(ShadowOf(m_player.get()), 4.0f);
    }
}

void ShadowMotion::UpdateVisibility() {
    bool shown = true;
    if (m_player.get()->IsWeaponHidden()) {
        shown = false;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x18)) {
        if (++m_blinkCount & 1) {
            shown = false;
        }
    }
    if (shown) {
        Show();
    } else {
        Hide();
    }
}

/* The node callbacks (see LoadMotion). */
static void HeadCallback(void *data);
static void HoldCallback(void *data);
static void AimCallback(void *data);
static void RightArmCallback(void *data);
static void RightForearmCallback(void *data);
static void RightHandCallback(void *data);
static void LeftArmCallback(void *data);
static void LeftForearmCallback(void *data);
static void LeftHandCallback(void *data);
static void Slot2HandCallback(void *data);
static void Slot2ForearmCallback(void *data);
static void Slot2ArmCallback(void *data);
static void Slot3HandCallback(void *data);
static void Slot3ForearmCallback(void *data);
static void Slot3ArmCallback(void *data);

void ShadowMotion::LoadMotion() {
    void *loader = fn_800122D4();
    fn_80078C4C(m_player.get());
    void *data = fn_80012684(loader, m_files.m_common2.m_file, m_files.m_common2.m_index);
    m_motionBase = fn_80012608(loader, m_files.m_common1.m_file, m_files.m_common1.m_index);
    RpClump *clump = fn_8001277C(loader, m_files.m_main.m_file, m_files.m_main.m_index);
    if (clump) {
        clump = fn_80457090(clump);
        UnkStage_8003A840 *stage = fn_8003A840();
        int area = fn_80169350(fn_80169364(clump));
        SetClumpWorld(clump, stage->GetWorld(area));
        fn_80038198(clump, fn_800AA584(this));
    }
    m_work = fn_8041B908(data, 2);
    if (m_work) {
        if (clump) {
            m_part = new UnkClumpUpdater_80015A7C(clump, m_work);
        }
        fn_80415808(m_work, 0x19, RightHandCallback, this);
        fn_80415808(m_work, 0x1A, RightForearmCallback, this);
        fn_80415808(m_work, 0x1B, RightArmCallback, this);
        fn_80415808(m_work, 0x1C, AimCallback, this);
        fn_80415808(m_work, 0x15, LeftHandCallback, this);
        fn_80415808(m_work, 0x16, LeftForearmCallback, this);
        fn_80415808(m_work, 0x17, LeftArmCallback, this);
        fn_80415808(m_work, 0x18, HoldCallback, this);
        fn_80415808(m_work, 0x1E, HeadCallback, this);
        fn_80415808(m_work, 0x29, Slot2ArmCallback, this);
        fn_80415808(m_work, 0x2A, Slot2ForearmCallback, this);
        fn_80415808(m_work, 0x2B, Slot2HandCallback, this);
        fn_80415808(m_work, 0x2D, Slot3ArmCallback, this);
        fn_80415808(m_work, 0x2E, Slot3ForearmCallback, this);
        fn_80415808(m_work, 0x2F, Slot3HandCallback, this);
    }
}

void ShadowMotion::Unload() {
    RpClump *clump = 0;
    if (m_part) {
        clump = *reinterpret_cast<RpClump **>(m_part);
        delete m_part;
    }
    if (m_work) {
        fn_8041B884(m_work);
    }
    if (clump) {
        void *world = fn_8046256C(clump);
        if (world) {
            fn_804623EC(world, clump);
        }
        fn_80457468(clump);
    }
    m_part = 0;
    m_work = 0;
}

void ShadowMotion::BindNodes() {
    if (m_work) {
        int set;
        switch (KindOf(m_player.operator->())) {
        case 0xE:
        case 0xF:
            set = 0;
            break;
        case 0x10:
        case 0x11:
            set = 1;
            break;
        default:
            set = 2;
            break;
        }
        const int *nodes = s_nodes[set];
        for (int i = 0; i < 14; i++) {
            int id = nodes[i];
            u8 valid = 1;
            const Mtx34f *matrix;
            switch (id) {
            case -1:
                matrix = &m_bodyMatrix;
                break;
            case -2:
                matrix = &m_baseMatrix;
                break;
            default:
                matrix = fn_80415E70(m_work, id);
                if (matrix == 0) {
                    matrix = m_hand->GetNodeMatrix(id);
                    valid = 0;
                }
                break;
            }
            m_nodes[i].m_matrix = matrix;
            m_nodes[i].m_valid = valid;
        }
    }
}

void ShadowMotion::OnIK(IKEventArgs &args) {
    Mtx34f matrix;
    fn_80016370(&matrix, args.m_node->m_frame);
    Vec3f target;
    fn_800091D0(&target, &Ref(fn_80079628(m_player.get(), &s_aimOffset)));
    fn_8040CFF8(&target, &Ref(fn_8040FB08(&matrix)));
    fn_800091D0(args.m_target, &target);
}

void ShadowMotion::InstallIK() {
    SetIKCallback(0, IKCallback(boost::bind(&ShadowMotion::OnIK, this, _1)));
}

void ShadowMotion::StartMotion() {
    switch (m_step) {
    case 0:
        if (m_playingMotion == m_nextMotion) {
            fn_800A9F9C(this, 0.0f);
        }
        m_step++;
        break;
    }
}

void ShadowMotion::WaitForPlayer() {
    switch (m_step) {
    case 1:
        const MotionEntry *entry = &s_motions[m_motionId];
        if (entry->m_nextIndex >= 0) {
            if (fn_800A9E1C(m_player)) {
                m_nextMotion = entry->m_nextIndex + (m_motionBase - 1);
                m_nextMotionArg = -1;
                m_step++;
            }
        } else {
            m_step++;
        }
        break;
    }
}

void ShadowMotion::Stand() {
    switch (m_step) {
    case 0:
        if ((u32)m_playingMotion == m_motionBase + 0x4A || (u32)m_playingMotion == m_motionBase + 0x4B) {
            m_step = 2;
            break;
        }
    case 1: {
        int motion = 0x67;
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
            switch (m_player.get()->m_weapon->GetWeaponKind()) {
            case 5:
                motion = 0x69;
                break;
            default:
                motion = 0x6B;
                break;
            }
        }
        m_nextMotion = m_motionBase - 1 + motion;
        m_step = 3;
        break;
    }
    case 2:
        if (fn_800A9E1C(m_player)) {
            m_step = 1;
        }
    case 3:
        if (m_player.get()->m_weapon && m_player.get()->m_weapon->IsFiring()) {
            if (fn_8005B904(m_player.get()->m_weapon->GetWeaponType()) > 0.7f) {
                m_nextMotion = m_motionBase + 0x4A;
            } else {
                m_nextMotion = m_motionBase + 0x4B;
            }
            if (m_step == 2) {
                fn_800A9F9C(this, 0.0f);
            }
            m_step = 2;
        }
        break;
    }
    if (RunSpeed() < 0.0f) {
        if (RunSpeed() < -1.5f) {
            m_nextMotion = m_motionBase + 0x69;
        } else {
            m_nextMotion = m_motionBase + 0x67;
            if (m_hand) {
                m_hand->SetPose(3, 0.0f);
            }
        }
        RunSpeed() = 0.0f;
        m_step = 2;
    }
    m_nextMotionArg = -1;
    m_moved = 0;
}

void ShadowMotion::SelectRunMotion() {
    int armed = fn_8006CD78(m_player.get()->m_stateBits, 0x1E) ? 1 : 0;
    float speed = RunSpeed();
    int a;
    int b;
    int step;
    int pose;
    if (speed < 1.0f) {
        RunBlend() = speed;
        step = 1;
        m_speed = 1.0f;
        a = s_runMotions[armed][0][0];
        b = s_runMotions[armed][0][1];
        pose = 1;
    } else if (speed < 2.0f) {
        RunBlend() = speed - 1.0f;
        step = 2;
        pose = 1;
        a = s_runMotions[armed][1][0];
        m_speed = 1.0f;
        b = s_runMotions[armed][1][1];
    } else if (speed < 3.0f) {
        RunBlend() = speed - 2.0f;
        step = 3;
        pose = 0;
        a = s_runMotions[armed][2][0];
        m_speed = 1.0f;
        b = s_runMotions[armed][2][1];
    } else {
        RunBlend() = speed - 3.0f;
        a = s_runMotions[armed][3][0];
        b = s_runMotions[armed][3][1];
        RunBlend() = RunBlend() < 1.0f ? RunBlend() : 1.0f;
        step = 4;
        m_speed = 1.0f;
        pose = 0;
    }
    m_nextMotion = m_motionBase - 1 + a;
    if (b >= 0) {
        m_nextMotionArg = m_motionBase - 1 + b;
    } else {
        RunBlend() = 0.0f;
        m_nextMotionArg = -1;
    }
    if (m_step != step) {
        if (m_step > 0) {
            m_blendTime = 0.0f;
            float frame = fn_800AA064(this);
            fn_800AA404(this);
            fn_800A9F9C(this, frame);
        }
        if (m_hand) {
            m_hand->SetPose(pose, 10.0f);
        }
        m_step = step;
    }
}

/* NonMatching (99.6%): the end differs only in register allocation (base - 1 is kept in r4 in
 * retail). */
void ShadowMotion::Walk() {
    Vec3f front;
    fn_800091EC(&front, &Ref(fn_800796DC(m_player.get())));
    Vec3f facing;
    fn_800091EC(&facing, &Ref(fn_8040CEE8(&Fields(m_player.get())->m_front)));
    Vec3f side;
    fn_800091EC(&side, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&Fields(m_player.get())->m_normal, &facing)))));
    float dot = fn_8040CF90(&front, &facing);
    int a;
    int b;
    if (dot > 0.0f) {
        a = 0x88;
        RunBlend() = 1.0f - dot;
        if (fn_8040CF90(&front, &side) > 0.0f) {
            b = 0x8F;
        } else {
            b = 0x8D;
        }
    } else if (dot > -0.5f) {
        if (fn_8040CF90(&front, &side) > 0.0f) {
            a = 0x8C;
            b = 0x8F;
        } else {
            a = 0x8B;
            b = 0x8D;
        }
        RunBlend() = 2.0f * (0.5f + dot);
    } else {
        a = 0x8A;
        if (fn_8040CF90(&front, &side) > 0.0f) {
            b = 0x8C;
        } else {
            b = 0x8B;
        }
        RunBlend() = 2.0f * (1.0f + dot);
    }
    int base = m_motionBase - 1;
    m_speed = RunSpeed();
    int next = a + base;
    int nextArg = b + base;
    if (m_nextMotion != next || m_nextMotionArg != nextArg) {
        m_nextMotion = next;
        m_nextMotionArg = nextArg;
        float frame = fn_800AA064(this);
        fn_800AA404(this);
        fn_800A9F9C(this, frame);
    }
}

void ShadowMotion::StartWalk() {
    switch (m_step) {
    case 0: {
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
            m_nextMotion = m_motionBase + 0x7A;
            m_nextMotionArg = -1;
        } else {
            m_nextMotion = m_motionBase + 0x6D;
            m_nextMotionArg = m_motionBase + 0x75;
        }
        Vec3f front;
        fn_800091EC(&front, &Ref(fn_800796DC(m_player.get())));
        RunBlend() = fn_8040CF90(&front, &Fields(m_player.get())->m_front) / 700.0f;
        if (RunBlend() > 1.0f) {
            RunBlend() = 1.0f;
        }
        m_step++;
        break;
    }
    }
}

void ShadowMotion::Fall() {
    switch (m_step) {
    case 0:
        if ((u32)m_nextMotion == m_motionBase + 0x70) {
            m_step = 100;
            break;
        }
    case 1: {
        UnkShadow_800B3538 *shadow = ShadowOf(m_player.get());
        if (!shadow->m_on || shadow->m_height >= s_landHeight) {
            m_nextMotion = m_motionBase + 0x71;
            m_step = 500;
        } else {
            m_nextMotion = m_motionBase + 0x6B;
        }
        break;
    }
    case 100:
        if (fn_800A9E1C(m_player)) {
            m_step = 1;
        }
        break;
    case 500:
        break;
    }
    m_nextMotionArg = -1;
}

void ShadowMotion::Land() {
    switch (m_step) {
    case 0:
        if ((u32)m_playingMotion != m_motionBase + 0x28) {
            if ((u32)m_playingMotion == m_motionBase + 0x71) {
                m_nextMotion = m_motionBase + 0x74;
            } else {
                m_nextMotion = m_motionBase + 0x6C;
            }
        }
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
}

void ShadowMotion::TurnToPlayer() {
    switch (m_step) {
    case 0: {
        Vec3f direction;
        UnkShadowFields_800B10CC *player = Fields(m_player.get());
        fn_8040F990(0, &direction, &player->m_front, &player->m_up);
        fn_8040CE74(&direction);
        switch (fn_80093C60(&m_player, &direction)) {
        case 0:
        case 3:
            m_nextMotion = m_motionBase + 0x6F;
            break;
        case 1:
            m_nextMotion = m_motionBase + 0x8F;
            break;
        case 2:
            m_nextMotion = m_motionBase + 0x8D;
            break;
        }
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
    case 1:
        if (fn_800A9E1C(m_player)) {
            m_nextMotion = m_motionBase + 0x6B;
            m_blendTime = 10.0f;
            m_step++;
        }
        break;
    }
}

void ShadowMotion::Step800B334C() {
    switch (m_step) {
    case 0:
        if ((u32)m_nextMotion == m_motionBase + 0x70) {
            m_step = 2;
            break;
        }
        if ((u32)m_nextMotion == m_motionBase + 0x6D && (u32)m_nextMotionArg == m_motionBase + 0x75) {
            m_nextMotion = m_motionBase + 0x6E;
            m_nextMotionArg = m_motionBase + 0x76;
        } else {
            m_nextMotion = m_motionBase + 0x6E;
            m_nextMotionArg = -1;
        }
        m_blendTime = 0.0f;
        m_speed = 1.0f;
        m_step = 1;
        break;
    }
}

void ShadowMotion::Step800B32B4() {
    switch (m_step) {
    case 1:
        if (m_playingMotion == m_nextMotion && fn_800A9E1C(m_player)) {
            fn_8006127C(&Fields(m_player.get())->m_motionFlags, 3);
            m_nextMotion = m_motionBase + 0x70;
            m_nextMotionArg = -1;
            m_blendTime = 0.0f;
            m_speed = 1.0f;
            m_step++;
        }
        break;
    }
}

void ShadowMotion::Step800B31E0() {
    int motion = -1;
    switch (m_step) {
    case 0:
        motion = (u32)m_nextMotion == m_motionBase + 0x38 ? 0x37 : 0x2C;
        m_step++;
        break;
    case 1:
        if (fn_800A9E1C(m_player)) {
            motion = 0x37;
        }
        if (RunSpeed() > 0.0f) {
            motion = 0x3A;
        } else if (RunSpeed() < 0.0f) {
            motion = 0x38;
        }
        break;
    }
    if (motion >= 0) {
        m_nextMotion = m_motionBase - 1 + motion;
    }
    m_nextMotionArg = -1;
    RunSpeed() = 0.0f;
}

void ShadowMotion::Step800B30F8() {
    int motion = -1;
    switch (m_step) {
    case 0:
        motion = 0x30;
        m_step++;
        break;
    case 1:
        if (fn_800A9E1C(m_player)) {
            motion = 0x35;
        }
        if (RunSpeed() > 0.0f) {
            motion = 0x36;
        } else if (RunSpeed() < 0.0f) {
            motion = 0x33;
            m_step = 100;
        }
        break;
    case 100:
        if (!(RunSpeed() < 0.0f)) {
            motion = 0x34;
            m_step = 1;
        }
        break;
    }
    if (motion >= 0) {
        m_nextMotion = m_motionBase - 1 + motion;
    }
    m_nextMotionArg = -1;
    RunSpeed() = 0.0f;
}

void ShadowMotion::Step800B2F48() {
    int motion = -1;
    switch (m_step) {
    case 0:
        if (RunSpeed() < 0.0f) {
            if (m_prevMotionId == 0x20 || m_prevMotionId == 0x17) {
                m_step = 300;
            } else {
                m_step = 301;
                motion = 0x5D;
            }
        } else {
            if (m_prevMotionId == 0x20 || m_prevMotionId == 0x17) {
                m_step = 100;
            } else {
                m_step = 101;
                motion = 0x63;
            }
        }
        break;
    case 100:
        motion = 0x54;
    case 101:
        if (fn_800A9E1C(m_player)) {
            m_step = 100;
        }
        if (RunSpeed() < 0.0f) {
            m_step = 200;
        }
        break;
    case 200:
        m_blendTime = 0.0f;
        m_step = 301;
        motion = 0x61;
        break;
    case 300:
        motion = 0x58;
    case 301:
        if (fn_800A9E1C(m_player)) {
            m_step = 300;
        }
        if (RunSpeed() > 0.0f) {
            m_step = 400;
        }
        break;
    case 400:
        m_blendTime = 0.0f;
        m_step = 101;
        motion = 0x62;
        break;
    }
    if (motion > 0) {
        m_nextMotion = m_motionBase - 1 + motion;
    }
    m_nextMotionArg = -1;
}

/* NonMatching (90%): retail keeps the motion in r5 and interleaves the step increment with the
 * id's sum; no source order tried reproduces it. */
void ShadowMotion::Step800B2EA4() {
    int next = m_nextMotion;
    switch (m_step) {
    case 0: {
        int motion = RunSpeed() > 0.0f ? 0x90 : 0x8E;
        m_step++;
        next = m_motionBase - 1 + motion;
        break;
    }
    case 1:
        if (fn_800A9E1C(m_player)) {
            m_step = 200;
        }
        break;
    }
    m_nextMotion = next;
    m_nextMotionArg = -1;
}

void ShadowMotion::Step800B2E28() {
    switch (m_step) {
    case 0: {
        int motion = fn_8006CD78(m_player.get()->m_stateBits, 2) ? 0x86 : 0x7C;
        m_nextMotion = m_motionBase - 1 + motion;
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
    }
}

void ShadowMotion::Step800B2D38() {
    int motion = -1;
    switch (m_step) {
    case 0:
        motion = 0x4F;
        m_step++;
        break;
    case 1:
        if (fn_800A9E1C(m_player)) {
            motion = 0x4D;
            m_step++;
        }
        break;
    case 2:
        fn_8006127C(&Fields(m_player.get())->m_motionFlags, 3);
        if (RunSpeed() != 0.0f) {
            motion = RunSpeed() > 0.0f ? 0x50 : 0x4E;
            m_step++;
        }
        break;
    }
    if (motion >= 0) {
        m_nextMotion = m_motionBase - 1 + motion;
        m_nextMotionArg = -1;
    }
    RunSpeed() = 0.0f;
}

void ShadowMotion::Idle() {
    int next = m_nextMotion;
    switch (m_step) {
    case 0:
        next = m_motionBase - 1 + s_idleMotions[(int)(100.0f * fn_8040EE90()) & 1];
        fn_800A9F9C(this, 0.0f);
        m_step++;
        break;
    case 1:
        if (fn_800A9E1C(m_player)) {
            next = m_motionBase + 0x6B;
            m_blendTime = 20.0f;
            m_step = 200;
        }
        break;
    }
    m_nextMotion = next;
    m_nextMotionArg = -1;
}

void ShadowMotion::Step800B2C0C() {
    switch (m_step) {
    case 0:
        if ((u32)m_nextMotion == m_motionBase + 0x5C || (u32)m_nextMotion == m_motionBase + 0x57 || (u32)m_nextMotion == m_motionBase + 0x59) {
            m_nextMotion = m_motionBase + 0x59;
        } else {
            m_nextMotion = m_motionBase + 0x55;
        }
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
}

void ShadowMotion::Step800B2B48() {
    m_nextMotionArg = -1;
    switch (m_step) {
    case 0:
        m_nextMotion = m_motionBase + 0x77;
        fn_800A9F9C(this, 0.0f);
        m_step++;
        break;
    case 1:
        if (fn_800A9E1C(m_player)) {
            m_nextMotion = m_motionBase + 0x78;
            m_step++;
        }
    case 2:
        if (RunSpeed() != 0.0f) {
            m_step = 0;
        }
        break;
    }
    RunSpeed() = 0.0f;
}

void ShadowMotion::Step800B2B44() {}

void ShadowMotion::WaitForLanding() {
    switch (m_step) {
    case 0:
        if (fn_800A9E1C(m_player)) {
            fn_8006127C(&Fields(m_player.get())->m_motionFlags, 3);
            m_nextMotion = m_motionBase + 0x43;
            m_step++;
        }
        break;
    case 1:
        fn_8006127C(&Fields(m_player.get())->m_motionFlags, 3);
        if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
            m_nextMotion = m_motionBase + 0x44;
            m_step++;
        }
        break;
    }
}

void ShadowMotion::Step800B2A18() {
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        m_nextMotion = m_motionBase + 0x81;
        m_nextMotionArg = -1;
    } else {
        m_nextMotion = m_motionBase + 0x82;
        m_nextMotionArg = -1;
    }
}

void ShadowMotion::Step800B2844() {
    Vec3f across;
    UnkShadowFields_800B10CC *player = Fields(m_player.get());
    fn_8040F990(0, &across, &player->m_front, &player->m_normal);
    float length = fn_8040CF50(&across);
    if (length <= 15.0f) {
        m_step = 0;
    } else {
        m_step = 1;
    }
    if (m_step == 0) {
        m_blendTime = 30.0f;
        m_nextMotion = field_0x3E4 + 0xC;
        m_nextMotionArg = -1;
    } else {
        fn_8040CE74(&across);
        Vec3f front;
        fn_800091EC(&front, &Ref(fn_800796DC(m_player.get())));
        Vec3f side;
        fn_800091EC(&side, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&Fields(m_player.get())->m_normal, &across)))));
        float dot = fn_8040CF90(&front, &across);
        RunBlend() = 1.0f - std::fabs(dot);
        int a;
        if (dot > 0.0f) {
            if (length <= 100.0f) {
                a = 0xE;
            } else {
                a = 0x12;
            }
        } else {
            a = 0xF;
        }
        int b;
        if (fn_8040CF90(&front, &side) > 0.0f) {
            b = 0x11;
        } else {
            b = 0x10;
        }
        int next = a + (field_0x3E4 - 1);
        int nextArg = b + (field_0x3E4 - 1);
        if (m_nextMotion != next || m_nextMotionArg != nextArg) {
            m_blendTime = 30.0f;
            m_nextMotion = next;
            m_nextMotionArg = nextArg;
            float frame = fn_800AA064(this);
            fn_800AA404(this);
            fn_800A9F9C(this, frame);
        }
    }
}

void ShadowMotion::Step800B2814() {
    switch (m_step) {
    case 0:
        m_nextMotion = field_0x3E4 + 0xA;
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
}

void ShadowMotion::Step800B27E4() {
    switch (m_step) {
    case 0:
        m_nextMotion = field_0x3E4 + 0xB;
        m_nextMotionArg = -1;
        m_step++;
        break;
    }
}

/* Aims an arm's hand at a point (names inferred): the slot's matrix or callback gives the point,
 * otherwise the aim's target. */
static void HandIK(UnkNodeArgs_800B10CC *args, int side) {
    ShadowMotion *motion = args->m_motion;
    UnkIKSlot_800B1264 *slot = &motion->m_ik[side];
    if (slot->m_weight != 0.0f) {
        if (slot->m_active) {
            Vec3f target;
            if (slot->m_callback.IsSet()) {
                IKEventArgs event;
                event.m_node = reinterpret_cast<UnkIKNode *>(args);
                event.m_target = &target;
                slot->m_callback.Call(event);
            } else {
                fn_800091D0(&target, &Ref(fn_8040FB08(&slot->m_matrix)));
            }
            if (side == 1) {
                Mtx34f pole;
                fn_8040C148(&pole, &RefM(((TranslationFn)fn_8040B034)(
                                       &Ref(fn_8040FB08(reinterpret_cast<Mtx34f *>(args->m_frame))))));
                MultiplyMatrix(pole, Fields(motion->m_player.operator->())->m_matrix);
                fn_8040B80C(&pole, Fields(motion->m_player.get())->m_rotation.z);
                fn_8040B884(&pole, Fields(motion->m_player.get())->m_rotation.x);
                fn_8040B848(&pole, Fields(motion->m_player.get())->m_rotation.y);
                fn_8040B80C(&pole, -40.0f * (lbl_805FAD38 / 180.0f));
                fn_80015DBC(&slot->m_bones[0], &slot->m_bones[1], &slot->m_bones[2], &target, &pole, 1.4f,
                            0.5f, 1.35103f);
            } else {
                Mtx34f pole;
                fn_8040C148(&pole, &RefM(((TranslationFn)fn_8040B034)(
                                       &Ref(fn_8040FB08(reinterpret_cast<Mtx34f *>(args->m_frame))))));
                MultiplyMatrix(pole, Fields(motion->m_player.operator->())->m_matrix);
                fn_8040B80C(&pole, Fields(motion->m_player.get())->m_rotation.z);
                fn_8040B884(&pole, Fields(motion->m_player.get())->m_rotation.x);
                fn_8040B848(&pole, Fields(motion->m_player.get())->m_rotation.y);
                fn_8040B848(&pole, 90.0f * (lbl_805FAD38 / 180.0f));
                fn_80015B60(&slot->m_bones[0], &slot->m_bones[1], &slot->m_bones[2], &target, &pole, 1.4f,
                            0.5f, 1.35103f);
                Mtx34f scale;
                fn_8040C148(&scale, &lbl_805E0720);
                fn_8040B720(&scale, 1.0f, 1.0f, 1.2f * slot->m_weight);
                fn_8040BAC4(&slot->m_bones[2], &scale);
            }
            Mtx34f matrix;
            fn_80016370(&matrix, args->m_frame);
            Mtx34f blended;
            fn_8040C148(&blended, &RefM(fn_8040EFA4(&matrix, &slot->m_bones[0], slot->m_weight)));
            fn_800163F4(args->m_frame, &blended);
        } else {
            Mtx34f matrix;
            fn_80016370(&matrix, args->m_frame);
            Vec3f target;
            fn_800091EC(&target, &RefAim(motion->m_player.get()->m_target->GetTargetPosition()));
            Vec3f position;
            fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
            fn_8040FA20(&matrix, &Vec3f(0.0f, 0.0f, 0.0f));
            Vec3f front;
            fn_800091EC(&front, &Ref(fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, 1.0f))));
            Vec3f to;
            fn_800091D0(&to, &Ref(((VectorSubtractFn)fn_8040CD74)(&target, &position)));
            fn_8040CE74(&to);
            float dot = fn_8040CF90(&front, &to);
            if (dot < 0.99995f) {
                float angle = slot->m_weight * fn_80015D68(dot);
                Vec3f axis;
                fn_800091EC(&axis, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&front, &to)))));
                fn_8040C148(&matrix, &RefM(fn_8001E044(&RefM(fn_80049778(&Ref(fn_8040CEE8(&axis)), angle)), &matrix)));
            }
            fn_8040FA20(&matrix, &position);
            fn_800163F4(args->m_frame, &matrix);
        }
    }
}

/* Turns a forearm towards the aim's target, or places it from the slot's bones. */
static void ForearmIK(UnkNodeArgs_800B10CC *args, int side) {
    ShadowMotion *motion = args->m_motion;
    UnkIKSlot_800B1264 *slot = &motion->m_ik[side];
    if (slot->m_weight != 0.0f) {
        if (slot->m_active) {
            Mtx34f matrix;
            fn_80016370(&matrix, args->m_frame);
            Mtx34f blended;
            fn_8040C148(&blended, &RefM(fn_8040EFA4(&matrix, &slot->m_bones[1], slot->m_weight)));
            fn_800163F4(args->m_frame, &blended);
        } else {
            Mtx34f matrix;
            fn_80016370(&matrix, args->m_frame);
            Vec3f target;
            fn_800091EC(&target, &RefAim(motion->m_player.get()->m_target->GetTargetPosition()));
            Vec3f position;
            fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
            fn_8040FA20(&matrix, &Vec3f(0.0f, 0.0f, 0.0f));
            Vec3f front;
            fn_800091EC(&front, &Ref(fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, 1.0f))));
            if (slot->m_weight > 0.0f) {
                Vec3f to;
                fn_800091D0(&to, &Ref(((VectorSubtractFn)fn_8040CD74)(&target, &position)));
                fn_8040CE74(&to);
                float dot = fn_8040CF90(&front, &to);
                if (dot < 0.99995f) {
                    float angle = slot->m_weight * fn_80015D68(dot);
                    Vec3f axis;
                    fn_800091EC(&axis, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&front, &to)))));
                    fn_8040C148(&matrix, &RefM(fn_8001E044(&RefM(fn_80049778(&Ref(fn_8040CEE8(&axis)), angle)), &matrix)));
                }
            }
            fn_8040FA20(&matrix, &position);
            fn_800163F4(args->m_frame, &matrix);
        }
    }
}

/* Turns an upper arm towards the aim's target, or places it from the slot's bones. */
static void ArmIK(UnkNodeArgs_800B10CC *args, int side) {
    UnkIKSlot_800B1264 *slot = &args->m_motion->m_ik[side];
    if (slot->m_weight != 0.0f) {
        if (slot->m_active) {
            Mtx34f matrix;
            fn_80016370(&matrix, args->m_frame);
            Mtx34f blended;
            fn_8040C148(&blended, &RefM(fn_8040EFA4(&matrix, &slot->m_bones[2], slot->m_weight)));
            fn_800163F4(args->m_frame, &blended);
        } else {
            Vec3f target;
            fn_800091EC(&target, &RefAim(args->m_motion->m_player.get()->m_target->GetTargetPosition()));
            Mtx34f matrix;
            fn_80016370(&matrix, args->m_frame);
            Vec3f position;
            fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
            fn_8040FA20(&matrix, &Vec3f(0.0f, 0.0f, 0.0f));
            Vec3f front;
            fn_800091EC(&front, &Ref(fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, 1.0f))));
            Vec3f to;
            fn_800091D0(&to, &Ref(((VectorSubtractFn)fn_8040CD74)(&target, &position)));
            fn_8040CE74(&to);
            float dot = fn_8040CF90(&front, &to);
            if (dot < 0.99995f) {
                float angle = slot->m_weight * fn_80015D68(dot);
                Vec3f axis;
                fn_800091EC(&axis, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&front, &to)))));
                fn_8040C148(&matrix, &RefM(fn_8001E044(&RefM(fn_80049778(&Ref(fn_8040CEE8(&axis)), angle)), &matrix)));
            }
            fn_8040FA20(&matrix, &position);
            fn_800163F4(args->m_frame, &matrix);
        }
    }
}

static void RightHandCallback(void *data) {
    HandIK(static_cast<UnkNodeArgs_800B10CC *>(data), 1);
}

static void RightForearmCallback(void *data) {
    ForearmIK(static_cast<UnkNodeArgs_800B10CC *>(data), 1);
}

static void RightArmCallback(void *data) {
    ArmIK(static_cast<UnkNodeArgs_800B10CC *>(data), 1);
}

static void LeftHandCallback(void *data) {
    HandIK(static_cast<UnkNodeArgs_800B10CC *>(data), 0);
}

static void LeftForearmCallback(void *data) {
    ForearmIK(static_cast<UnkNodeArgs_800B10CC *>(data), 0);
}

static void LeftArmCallback(void *data) {
    ArmIK(static_cast<UnkNodeArgs_800B10CC *>(data), 0);
}

/* Blends a matrix's rotation towards another's (names inferred). It returns the matrix by value
 * (the callers pass the hidden result first). */
typedef Mtx34f (*BlendRotationFn)(const Mtx34f *from, const Mtx34f *to, float t);

static void BlendRotation(Mtx34f *out, const Mtx34f *from, const Mtx34f *to, float t) {
    if (t >= 1.0f) {
        fn_8040C148(out, to);
    } else {
        Mtx34f rotation;
        Quatf start;
        Quatf end;
        Vec3f front;
        Vec3f up;
        fn_8007A34C(&end, &RefQ(fn_8040C55C(to)));
        fn_800091D0(&up, &Ref(fn_8040B67C(from, &Vec3f(0.0f, 1.0f, 0.0f))));
        fn_800091D0(&front, &Ref(fn_8040B67C(from, &Vec3f(0.0f, 0.0f, -1.0f))));
        Vec3f angles = fn_8040F364(&(const Vec3f &)fn_8040CEE8(&front), &(const Vec3f &)fn_8040CEE8(&up));
        fn_8040C148(&rotation, &lbl_805E0720);
        fn_8040B848(&rotation, angles.y);
        fn_8040B884(&rotation, angles.x);
        fn_8040B80C(&rotation, angles.z);
        fn_8007A34C(&start, &RefQ(fn_8040C55C(&rotation)));
        ((QuatToMatrixFn)fn_8040B228)(out, &RefQ(fn_8040C3EC(&start, &end, t)));
    }
}

/* Faces the aim with the arm that holds a gun (node 0x1C). */
static void AimCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    ShadowMotion *motion = args->m_motion;
    if (fn_8006CD78(motion->m_player.get()->m_stateBits, 0x1F)) {
        Mtx34f matrix;
        fn_80016370(&matrix, args->m_frame);
        Vec3f position;
        fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
        fn_8040FA20(&matrix, &lbl_805E07A0);
        if (motion->m_ik[1].m_weight > 0.0f) {
            Vec3f up;
            Vec3f front;
            Vec3f side;
            Vec3f target;
            fn_800091EC(&target, &RefAim(motion->m_player.get()->m_target->GetTargetPosition()));
            fn_800091D0(&front, &Ref(fn_8040CEE8(&Ref(((VectorSubtractFn)fn_8040CD74)(&position, &target)))));
            fn_800091D0(&up, &Ref(fn_80016284(
                                  &Ref(motion->m_player.get()->m_target->ToAimSpace(Vec3f(0.0f, 1.0f, 0.0f))))));
            fn_800091D0(&side, &Ref(fn_8040CFB8(&up, &front)));
            fn_800091D0(&up, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&front, &side)))));
            Vec3f angles = fn_8040F364(&front, &up);
            Mtx34f rotation;
            fn_8040B6F8(&rotation);
            fn_8040B848(&rotation, angles.y);
            fn_8040B884(&rotation, angles.x);
            fn_8040B80C(&rotation, angles.z);
            fn_8040C148(&matrix, &RefM(((BlendRotationFn)BlendRotation)(&matrix, &rotation, motion->m_ik[1].m_weight)));
        }
        fn_8040FA20(&matrix, &position);
        fn_800163F4(args->m_frame, &matrix);
    }
}

/* Places the object the player holds (node 0x18). */
static void HoldCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    ShadowMotion *motion = args->m_motion;
    if (fn_8006CD78(motion->m_player.get()->m_stateBits, 0x1E)) {
        if (motion->m_player.get()->vf_0x4C().expired()) {
            return;
        }
        Mtx34f matrix;
        fn_80016370(&matrix, args->m_frame);
        Vec3f position;
        fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
        fn_8040FA20(&matrix, &Vec3f(0.0f, 0.0f, 0.0f));
        if (motion->m_ik[0].m_weight > 0.0f) {
            Vec3f front;
            fn_800091D0(&front, &Ref(fn_80016284(&Ref(fn_80079698(motion->m_player.get())))));
            Vec3f up;
            fn_800091D0(&up, &Ref(fn_800796DC(motion->m_player.get())));
            Vec3f side;
            fn_800091D0(&side, &Ref(fn_8040CFB8(&front, &up)));
            fn_800091D0(&front, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&up, &side)))));
            Vec3f angles = fn_8040F364(&up, &front);
            Mtx34f rotation;
            fn_8040B6F8(&rotation);
            fn_8040B848(&rotation, angles.y);
            fn_8040B884(&rotation, angles.x);
            fn_8040B80C(&rotation, angles.z);
            fn_8040C148(&matrix, &RefM(((BlendRotationFn)BlendRotation)(&matrix, &rotation, motion->m_ik[0].m_weight)));
        }
        fn_8040FA20(&matrix, &position);
        fn_800163F4(args->m_frame, &matrix);
        shared_ptr<PickupCharInfo> object = motion->m_player.get()->vf_0x4C().lock();
        Vec3f front;
        fn_800091D0(&front, &Ref(fn_80079698(motion->m_player.get())));
        Vec3f up;
        fn_800091D0(&up, &Ref(fn_800796DC(motion->m_player.get())));
        Vec3f angles = fn_8040F364(&up, &front);
        Mtx34f placed;
        fn_8040C148(&placed, &RefM(((TranslationFn)fn_8040B034)(&position)));
        fn_8040B848(&placed, angles.y);
        fn_8040B884(&placed, angles.x);
        fn_8040B80C(&placed, angles.z);
        fn_801E41C4(object.get(), &placed);
    }
}

/* Turns the head (node 0x1E). */
static void HeadCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    args->m_motion->m_head.Update(args, args->m_motion->m_dt);
}

/* The bones of slots 2 and 3, which carry an object in both hands (names inferred). */
static void Slot2ArmCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    UnkIKSlot_800B1264 *slot = &args->m_motion->m_ik[2];
    if (slot->m_active) {
        fn_80015DE0(&slot->m_bones[0], &slot->m_bones[1], 1.6750001f, 2.525f, &Ref(fn_8040FB08(&slot->m_matrix)),
                    args->m_frame);
        fn_800163F4(args->m_frame, &slot->m_bones[0]);
    }
}

static void Slot2ForearmCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    if (args->m_motion->m_ik[2].m_active) {
        fn_800163F4(args->m_frame, &args->m_motion->m_ik[2].m_bones[1]);
    }
}

static void Slot2HandCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    if (args->m_motion->m_ik[2].m_active) {
        fn_800163F4(args->m_frame, &args->m_motion->m_ik[2].m_matrix);
    }
}

static void Slot3ArmCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    UnkIKSlot_800B1264 *slot = &args->m_motion->m_ik[3];
    if (slot->m_active) {
        fn_80015DE0(&slot->m_bones[0], &slot->m_bones[1], 1.6750001f, 2.525f, &Ref(fn_8040FB08(&slot->m_matrix)),
                    args->m_frame);
        fn_800163F4(args->m_frame, &slot->m_bones[0]);
    }
}

static void Slot3ForearmCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    if (args->m_motion->m_ik[3].m_active) {
        fn_800163F4(args->m_frame, &args->m_motion->m_ik[3].m_bones[1]);
    }
}

static void Slot3HandCallback(void *data) {
    UnkNodeArgs_800B10CC *args = static_cast<UnkNodeArgs_800B10CC *>(data);
    if (args->m_motion->m_ik[3].m_active) {
        fn_800163F4(args->m_frame, &args->m_motion->m_ik[3].m_matrix);
    }
}

void UnkHeadTurn_800B1308::Update(UnkNodeArgs_800B10CC *args, float dt) {
    ShadowMotion *motion = args->m_motion;
    Vec2f angles(0.0f, 0.0f);
    Mtx34f matrix;
    fn_80016370(&matrix, args->m_frame);
    Quatf goal;
    float speed;
    if (fn_80014E34(&motion->m_flags.m_bits, 1) && motion->m_player.get()->m_target->vf_0x10()) {
        Vec3f local;
        Vec3f direction;
        Vec3f target;
        fn_800091D0(&target, &RefAim(motion->m_player.get()->m_target->vf_0x1C()));
        fn_800091D0(&local, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(&matrix)), &target)));
        fn_800091D0(&direction, &Ref(fn_8040CEE8(&local)));
        Vec2f turn = fn_8040F760(&direction);
        angles.x = turn.x;
        angles.y = turn.y;
        angles.x = fn_80070460(angles.x, -40.0f * (lbl_805FAD38 / 180.0f), 45.0f * (lbl_805FAD38 / 180.0f));
        angles.y = fn_80070460(angles.y, -45.0f * (lbl_805FAD38 / 180.0f), 45.0f * (lbl_805FAD38 / 180.0f));
        Mtx34f rotation;
        fn_8040B6F8(&rotation);
        fn_8040B848(&rotation, angles.y);
        fn_8040B884(&rotation, angles.x);
        fn_8007A34C(&goal, &RefQ(fn_8040C55C(&rotation)));
        speed = 200.0f * (lbl_805FAD38 / 180.0f);
    } else {
        fn_8040E988(&angles);
        fn_8040C908(&goal);
        speed = 400.0f * (lbl_805FAD38 / 180.0f);
    }
    Vec3f front;
    fn_800091EC(&front, &Ref(fn_8040B67C(&RefM(fn_8040B228(&m_rotation)), &Vec3f(0.0f, 0.0f, -1.0f))));
    Vec3f aim(0.0f, 0.0f, -1.0f);
    fn_800091D0(&aim, &Ref(fn_8040D118(&aim, angles.x)));
    fn_800091D0(&aim, &Ref(fn_8040D0AC(&aim, angles.y)));
    float angle = fn_80015D68(fn_8040CF90(&front, &aim));
    speed *= dt;
    float t;
    if (angle >= speed) {
        t = speed / angle;
        if (t >= 0.5f) {
            t = 0.5f;
        }
    } else {
        t = 0.5f;
    }
    fn_8007A34C(&m_rotation, &RefQ(fn_8040C3EC(&m_rotation, &goal, t)));
    fn_8040B8C0(&matrix, &m_rotation);
    fn_800163F4(args->m_frame, &matrix);
}

} // namespace Shadow
} // namespace Player

/* Creates Shadow's motion (Game/PlayerShadow.cpp calls it by its address). */
extern "C" void fn_800B10CC(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 *files) {
    new Player::Shadow::ShadowMotion(parent, player, files);
}
