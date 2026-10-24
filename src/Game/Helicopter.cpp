/* Helicopter (class name from RTTI): the Helicopter gadget, a helicopter (HELICOPTER.BIN) that
 * flies either back and forth along a straight line (move type 0: "MoveLength" X/Y/Z covered in
 * "MoveSec" seconds, then a pause of "PauseSec") or round a circle about its placement (move type
 * 1: "CircleRadius", "MoveSpd (point/sec)", "InitPos (deg)"), banking into its turns. It has two
 * landing skids, each with a target and a capsule collider: a player character that touches a
 * skid is hung on it with a TouchHangCommand until it rides off (RideCommand 0xB) or leaves
 * (LeaveCommand 0xC). Attacks wear its health down; once it is gone the attacker is rewarded and
 * the helicopter breaks into its debris, shaking the camera. File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit's code reaches its .data
 * through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include "Game/GadgetCharCollision.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The cursor a parameter's label callback receives (layout as in FootingMovable.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

extern "C" {
/* Helpers as in Game/GadgetUtil.h, which this unit does not include: it reads pi as the constant
 * Game/MathConstants.h declares. */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void *fn_80053164(void);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE60(Vec3f *vector);
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
}

extern u32 lbl_805F06D8; /* passed to the debug draw calls */

/* The vector type's out-of-line copy constructor and assignment (see GadgetUtil.h). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* True when the vector is shorter than 0.01 (name inferred). */
inline bool IsZero(const Vec3f &vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z < 0.0001f;
}

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* Moves target to position (name inferred). */
inline void MoveTarget(Target *target, const Vec3f &position) {
    target->fn_800CCE38(&position);
}

/* The parameters of a debris spawn (layout as in ArkCannon.cpp). */
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

/* A part of a model looked up by id (see BreakObj.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

/* A node of a collider's contact list, and an iterator over the list (as in BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* The 0x18-byte line segment fn_80020A20 builds from two points (name inferred), and the one
 * fn_80021DE4 converts it to (meaning unknown). */
struct UnkSegment_80020A20 {
    UnkSegment_80020A20() {}

    /* 0x00 */ u8 m_data[0x18]; /* the start and end points */
};
SIZE_ASSERT(UnkSegment_80020A20, 0x18);

struct UnkSegment_80021DE4 {
    u8 m_data[0x18];
};

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E10 clears it, fn_80014E1C
 * sets bit, fn_8006127C clears bit and fn_80014E34 tests bit (see FireRing.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Tells the owner that the gadget is gone (behaviour inferred; see SetVehicle.cpp). */
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);

/* GadgetModelInfo helpers (see BreakObj.cpp, ArkCannon.cpp and Catapult.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_8016082C(GadgetModelInfo *model, int arg1);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80162790(GadgetModelInfo *model);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* A test on a value (behaviour unknown; see FootingMovable.cpp). */
bool fn_8000A950(float value);

/* Plays sound id at a character info through the sound manager (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* The camera manager and a camera shake (see ElecBomb.cpp). */
void *fn_80009548(void);
void fn_8000D664(void *manager, int arg1, float arg2, float arg3, float arg4);

/* The collider manager; a capsule collider for charInfo (see CatchStick.cpp); and the handle
 * a collider is wrapped in, advanced by dt (behaviour inferred). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C78D4(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, float halfLength,
                                              float radius, const Vec3f *offset,
                                              const Vec3f *rotation, int arg9);
extern Vec3f lbl_805E07B8;
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);

/* The character manager's character by id, and the player's id (see SecretDoor.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
extern int lbl_805EE588;
/* The character info a collider belongs to (see BlockChange.cpp). */
boost::weak_ptr<CharInfo> fn_800C4568(void *manager,
                                      const boost::weak_ptr<CharaColliBase> &collider);
/* The contact list: its first and end nodes, the post-increment, and the collider of a node
 * (see BlockChange.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);

/* Debug drawing: a capsule round a segment, and an arc (behaviour inferred; the other arguments
 * are unknown). */
void fn_802E0AA0(void *segment, u32 *arg1, float radius);
void fn_802E0C60(const Vec3f *position, const Vec3f *rotation, int arg2,
                 float arg3, float radius);

/* Vector and matrix helpers (behaviour inferred from their bodies). */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* a x b */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);       /* turned about y */
/* Splits vector into its parts along and across normal (either output may be null). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector,
                 const Vec3f *normal);
/* The angle between two unit vectors. */
double fn_8040F8FC(const Vec3f *a, const Vec3f *b);
/* The rotation by angle about axis. */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
/* The Euler angles of a direction. */
Vec3f fn_8040F7F8(const Vec3f *direction);
/* The inverse of a matrix. */
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
/* The point of segment nearest to point. */
Vec3f fn_8040FC3C(const UnkSegment_80021DE4 &segment, const Vec3f *point);
UnkSegment_80021DE4 fn_80021DE4(const UnkSegment_80020A20 *segment);
/* value clamped to [low, high]. */
const float &fn_80070460(const float &value, const float &low, const float &high);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first argument;
 * Game/MathTypes.h declares it that way), and a + b. */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
extern "C" Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* Copies a rotation one angle at a time (name inferred). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Marks a new collider active (name and meaning inferred). */
inline void Activate(CharaColliBase *collider) {
    reinterpret_cast<u8 *>(collider)[0x2C] = 1;
}

/* The flag word at 0xA4 (struct name unknown). */
struct HelicopterFlags {
    HelicopterFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
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

    u32 m_bits; /* bit 0: destroyed, bit 1: a skid can catch a character, bit 2: one hangs */
};

/* The six parameters the owner supplies (names from the parameter table). */
struct HelicopterParams {
    /* 0x00 */ u32 moveType; /* 0 straight, 1 circle */
    /* 0x04 */ float param0;  /* MoveLengthX, or CircleRadius */
    /* 0x08 */ float param1;  /* MoveLengthY, or MoveSpd (point/sec) */
    /* 0x0C */ float param2;  /* MoveLengthZ, or InitPos (deg) */
    /* 0x10 */ float moveSec;
    /* 0x14 */ float pauseSec;
};

/* RideCommand (command type 0xB) is declared in Game/CharCommand.h. */

class Helicopter : public Gadget {
  public:
    Helicopter(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Helicopter();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model, the targets and the hang point (name inferred). */
    void RebuildTransform();
    /* The two move types (names inferred). */
    void UpdateStraight(float dt);
    void UpdateCircle(float dt);
    /* Turns the helicopter towards its motion and tilts it (name inferred). */
    void UpdateTilt(float dt);
    /* Hangs character on the skid (name inferred). */
    void HangCharacter(boost::shared_ptr<CharInfo> character);
    /* The skid colliders' callbacks (names inferred). */
    void OnColliDetectFront(ColliDetectArgs &args);
    void OnColliDetectBack(ColliDetectArgs &args);
    /* Catches the player when it touches a skid (name inferred). */
    void CatchPlayer(ColliDetectArgs &args, bool front);
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Lets the hanging character go (name inferred). */
    void LetGo();
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ HelicopterParams m_params;
    /* 0x48 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x50 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x58 */ boost::shared_ptr<GadgetCharCollision> m_frontHandle;
    /* 0x60 */ boost::shared_ptr<GadgetCharCollision> m_backHandle;
    /* 0x68 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x70 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x8C */ boost::shared_ptr<Target> m_target;
    /* 0x94 */ boost::shared_ptr<Target> m_frontTarget;
    /* 0x9C */ boost::shared_ptr<Target> m_backTarget;
    /* 0xA4 */ HelicopterFlags m_flags2;
    /* 0xA8 */ s32 m_moveState; /* straight: 0 moving, 1 paused */
    /* 0xAC */ s32 m_direction; /* straight: 0 out, 1 back */
    /* 0xB0 */ float m_timer;
    /* 0xB4 */ float m_health;
    /* 0xB8 */ float m_turnSpeed;
    /* 0xBC */ boost::weak_ptr<CharInfo> m_rider;
    /* 0xC4 */ Vec3f m_hangPosition;
    /* 0xD0 */ Vec3f m_hangRotation;
    /* 0xDC */ Vec3f m_hangOffset; /* the hang position in model space */
};
SIZE_ASSERT(Helicopter, 0xE8);

/* The skids' positions in model space, and the segments along them (names inferred). */
static Vec3f s_unused; /* never used; it starts the unit's .bss */
static Vec3f s_frontSkid(13.25f, -19.3f, 0.325f);
static Vec3f s_backSkid(-13.25f, -19.3f, 0.325f);
/* fn_8040CDD0 called through its result pointer (a + b). */
typedef void (*VectorAddOutFn)(float *result, const Vec3f *a, const float *b);

/* A skid's segment, 13.5 either side of the skid along z. The original builds it from temporary
 * vectors in the startup code; temporaries of the shared vector type would be constructed out of
 * line there under GC/2.7, so this constructor writes them out as arrays (as Cage.cpp does). */
struct SkidSegment {
    SkidSegment(const Vec3f *skid) {
        float endOffset[3];
        float endPoint[3];
        float startOffset[3];
        float startPoint[3];

        endOffset[0] = 0.0f;
        endOffset[1] = 0.0f;
        endOffset[2] = -13.5f;
        ((VectorAddOutFn)fn_8040CDD0)(endPoint, skid, endOffset);
        startOffset[0] = 0.0f;
        startOffset[1] = 0.0f;
        startOffset[2] = 13.5f;
        ((VectorAddOutFn)fn_8040CDD0)(startPoint, skid, startOffset);
        fn_80020A20(this, (const Vec3f *)startPoint, (const Vec3f *)endPoint);
    }

    const Vec3f *Start() const {
        return reinterpret_cast<const Vec3f *>(m_data);
    }
    const Vec3f *End() const {
        return reinterpret_cast<const Vec3f *>(m_data + 0xC);
    }

    /* 0x00 */ u8 m_data[0x18]; /* the start and end points */
};

static SkidSegment s_frontSegment(&s_frontSkid);
static SkidSegment s_backSegment(&s_backSkid);

const char *Helicopter::s_taskName = "Helicopter";

Helicopter::Helicopter(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_flags2(0), m_hangRotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x53, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Helicopter::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_target = fn_801E7128(m_chara, 0xA800);
    m_frontTarget = fn_801E7128(m_chara, 0x8400);
    m_backTarget = fn_801E7128(m_chara, 0x8400);
    m_model = fn_8015F800(1, "HELICOPTER.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    fn_8016082C(m_model.get(), 0);
    m_model->fn_8015FF60();
    m_model->fn_80160B18(0);
    fn_80161BD8(m_model.get(), 0, m_chara, 0);
    {
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&Helicopter::OnAttack, this, _1));
        }
    }
    {
        boost::shared_ptr<CharaColliBase> front =
            fn_800C78D4(fn_80021B08(), m_chara, 0x10000002, 7, 0, 13.5f, 1.0f, &s_frontSkid,
                        &lbl_805E07B8, 0);
        Activate(front.operator->());
        m_frontHandle = fn_8015B8C4(front);
        fn_800F5980(m_frontHandle.get(), boost::bind(&Helicopter::OnColliDetectFront, this, _1));
        boost::shared_ptr<CharaColliBase> back =
            fn_800C78D4(fn_80021B08(), m_chara, 0x10000002, 7, 0, 13.5f, 1.0f, &s_backSkid,
                        &lbl_805E07B8, 0);
        Activate(back.operator->());
        m_backHandle = fn_8015B8C4(back);
        fn_800F5980(m_backHandle.get(), boost::bind(&Helicopter::OnColliDetectBack, this, _1));
    }
    RebuildTransform();
    m_particles = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particles;
        fn_8015FA78(m_model.get(), fn_80169D00, &ref);
        fn_80167010(m_particles.px, -1);
    }
    m_sound = fn_801D3DC8(0x713B, m_chara.get(), this);
}

Helicopter::~Helicopter() {
    m_sound.fn_801D455C();
}

extern "C" void fn_802F8594(void) {
    fn_8006B474("HELICOPTER.BIN", 1);
}

extern "C" void fn_802F8590(void) {}

void Helicopter::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Helicopter(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void Helicopter::Behave(float dt) {
    if (m_flags2.Test(0)) {
        if (m_particles.get() != 0) {
            fn_80166F88(m_particles.px);
            m_particles.reset();
        }
        fn_80162790(m_model.get());
        m_model->fn_80161880();
        {
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);
            UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), 1000);

            debris.model = fn_80012700(fn_800122D4(), 1, part->name);
            debris.field_0x40 = m_chara->m_position.y;
            debris.scale = 1.0f;
            debris.field_0x4A = 0;
            debris.field_0x54 = 1;
            fn_8015D93C(&debris);
        }
        fn_8000D664(fn_80009548(), 0, 0.7f, 1.6f, 8.0f);
        LetGo();
        m_sound.fn_801D455C();
        fn_801D54F4(fn_80053164(), 0x713C, m_chara.get(), 0, 1);
        fn_800C9AD4(m_owner);
        m_flags |= 1;
    } else {
        Vec3f delta;

        fn_800091EC(&delta, &m_chara->m_position);
        switch (m_params.moveType) {
        case 0:
            UpdateStraight(dt);
            break;
        case 1:
            UpdateCircle(dt);
            break;
        }
        fn_800091D0(&delta, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_chara->m_position, &delta));
        if (fn_8000A950(dt)) {
            fn_800091D0(&m_chara->field_0x18, &lbl_805E07A0);
        } else {
            AssignVector(m_chara.px->field_0x18, fn_8040CDA8(&delta, 1.0f / dt));
        }
        UpdateTilt(dt);
    }
    if (m_flags2.Test(2)) {
        boost::shared_ptr<CharInfo> rider = m_rider.lock();
        if (rider.get() != 0) {
            HangCharacter(rider);
        } else {
            LetGo();
        }
    }
    RebuildTransform();
    m_model->fn_80162FB0(dt);
    fn_8015BE9C(m_frontHandle.get(), dt);
    fn_8015BE9C(m_backHandle.get(), dt);
    if (m_particles.get() != 0) {
        fn_8016761C(m_particles.px, dt);
    }
}

void Helicopter::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    switch ((s32)m_params.moveType) {
    case 0: {
        UnkSegment_80020A20 segment;
        fn_80020A20(&segment, &m_chara.px->m_position,
                    &(const Vec3f &)fn_8040CDD0(
                        &m_chara->m_position, &Vec3f(m_params.param0, m_params.param1, m_params.param2)));
        fn_802E0AA0(&segment, &lbl_805F06D8, 5.0f);
        break;
    }
    case 1: {
        Vec3f center;
        fn_800C9F14(owner, &center);
        fn_802E0C60(&center, &Vec3f(0.5f * lbl_805FAD38, 0.0f, 0.0f), 0, 2.5f, m_params.param0);
        break;
    }
    }
}

void Helicopter::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_chara->m_rotation.x = 0.0f;
    m_chara->m_rotation.z = 0.0f;
    fn_8040CE60(&m_chara->field_0x18);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (m_params.moveType == 1) {
        Vec3f offset;

        fn_800091EC(&offset, &Vec3f(0.0f, 0.0f, -m_params.param0));
        fn_800091D0(&offset, &(const Vec3f &)fn_8040D0AC(&offset, DegToRad(m_params.param2)));
        AddVector(m_chara->m_position, offset);
        if (!fn_8000A950(m_params.param0) && !fn_8000A950(m_params.param1)) {
            Vec3f direction;

            fn_800091EC(&direction, &(const Vec3f &)fn_8040CFB8(&offset, &Vec3f(0.0f, 1.0f, 0.0f)));
            fn_8040CE74(&direction);
            if (m_params.param1 < 0.0f) {
                fn_8040CE38(&direction, -1.0f);
            }
            float y = fn_8040F7F8(&direction).y;
            m_chara->m_rotation.y = y;
        }
    }
    m_flags2.Clear();
    m_moveState = 0;
    m_direction = 0;
    m_timer = 0.0f;
    m_health = 10.0f;
    m_turnSpeed = 0.0f;
}

void Helicopter::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
    MoveTarget(m_frontTarget.px, fn_8040B67C(&matrix, &s_frontSkid));
    MoveTarget(m_backTarget.px, fn_8040B67C(&matrix, &s_backSkid));
    if (m_flags2.Test(2)) {
        AssignVector(m_hangPosition, fn_8040B67C(&matrix, &m_hangOffset));
        Vec3f direction;

        fn_800091EC(&direction,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_hangPosition, &m_chara->m_position));
        direction.y = 0.0f;
        fn_8040CE74(&direction);
        CopyRotation(m_hangRotation, fn_8040F7F8(&direction));
    }
}

void Helicopter::UpdateStraight(float dt) {
    m_timer += dt;
    switch (m_moveState) {
    case 0:
        if (m_timer < m_params.moveSec && !fn_8000A950(m_params.moveSec)) {
            float t = m_timer / m_params.moveSec;
            float c = std::cos(t * lbl_805FAD38);
            t = 0.5f * (1.0f - c);
            Vec3f length(m_params.param0, m_params.param1, m_params.param2);

            fn_800C9F14(m_owner, &m_chara->m_position);
            if (m_direction == 0) {
                AddVector(m_chara->m_position, fn_800091A4(&length, t));
            } else {
                AssignVector(m_chara.px->m_position,
                             fn_8040CDD0(&m_chara.px->m_position,
                                                        &(const Vec3f &)fn_8040CDA8(&length, 1.0f - t)));
            }
        } else {
            m_timer = 0.0f;
            m_moveState = 1;
        }
        break;
    case 1:
        if (m_timer > m_params.pauseSec) {
            m_timer = 0.0f;
            if (m_direction == 0) {
                m_direction = 1;
            } else {
                m_direction = 0;
            }
            m_moveState = 0;
        }
        break;
    }
}

void Helicopter::UpdateCircle(float dt) {
    if (!fn_8000A950(m_params.param1)) {
        Vec3f center;
        Vec3f offset;

        fn_800C9F14(m_owner, &center);
        float angle = m_params.param1 * dt / m_params.param0;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_chara->m_position, &center));
        fn_8040CE74(&offset);
        fn_800091D0(&offset, &(const Vec3f &)fn_8040D0AC(&offset, angle));
        fn_8040CE38(&offset, m_params.param0);
        AssignVector(m_chara.px->m_position, fn_8040CDD0(&center, &offset));
    }
}

/* NonMatching (98.8%): retail stores angle (stfs) before rounding fn_8040F8FC's result for
 * fn_8000A950 (frsp); GC/2.7 rounds first. Every source shape tried keeps GC/2.7's order, which
 * looks like the float conversion differences in docs/research-notes.md. */
void Helicopter::UpdateTilt(float dt) {
    if (!IsZero(m_chara->field_0x18)) {
        Vec3f horizontal;
        Vec3f up(0.0f, 1.0f, 0.0f);

        fn_8040F990(0, &horizontal, &m_chara.px->field_0x18, &up);
        if (!IsZero(horizontal)) {
            Vec3f forward(0.0f, 0.0f, 1.0f);

            fn_800091D0(&forward, &(const Vec3f &)fn_8040D0AC(&forward, m_chara.px->m_rotation.y));
            float angle = fn_8040F8FC(&forward, &horizontal);
            if (!fn_8000A950(angle)) {
                m_turnSpeed += dt * DegToRad(10.0f);
                m_turnSpeed = fn_80070460(m_turnSpeed, 0.0f, DegToRad(15.0f));
                angle = fn_80070460(angle, 0.0f, m_turnSpeed * dt);
                Vec3f axis;

                fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&forward, &horizontal));
                if (IsZero(axis)) {
                    fn_800091D0(&axis, &up);
                }
                fn_800091D0(&forward, &(const Vec3f &)fn_8040B67C(
                                          &(const Mtx34f &)fn_8040FD8C(&axis, angle), &forward));
                GadgetCharInfo *charInfo = m_chara.px;
                charInfo->m_rotation.y = fn_8040F7F8(&forward).y;
            } else {
                m_turnSpeed = 0.0f;
            }
            float tilt = fn_8040CF50(&horizontal) / 60.0f;
            tilt = fn_80070460(tilt, 0.0f, 1.0f);
            m_chara.px->m_rotation.x = tilt * DegToRad(16.0f);
        } else {
            m_chara.px->m_rotation.x = 0.0f;
        }
    }
}

void Helicopter::HangCharacter(boost::shared_ptr<CharInfo> character) {
    character->PushCommand(new Player::TouchHangCommand(m_chara, &m_hangPosition, &m_hangRotation, 3, 4));
}

void Helicopter::OnColliDetectFront(ColliDetectArgs &args) {
    CatchPlayer(args, true);
}

void Helicopter::OnColliDetectBack(ColliDetectArgs &args) {
    CatchPlayer(args, false);
}

/* NonMatching (99.9%): retail sets fn_8040FC3C's result pointer (r3) before the segment (r4); with
 * the segment held by reference GC/2.7 sets them the other way round, and writing the call nested
 * (which orders them right) gives the result temporary the wrong stack slot. */
void Helicopter::CatchPlayer(ColliDetectArgs &args, bool front) {
    if (args.m_contacts != 0) {
        boost::shared_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
        for (UnkColliContactIter_8001EDFC it = fn_8001EE04(args.m_contacts); it.m_node != fn_8001EDEC(args.m_contacts); fn_80027374(&it, 0)) {
            boost::shared_ptr<CharInfo> hit =
                fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
            if (hit.get() != 0 && player == hit) {
                Mtx34f matrix;

                fn_8040C148(&matrix, &lbl_805E0720);
                fn_8040B59C(&matrix, &m_chara->m_position);
                fn_8040B848(&matrix, m_chara->m_rotation.y);
                fn_8040B884(&matrix, m_chara->m_rotation.x);
                const SkidSegment *skid = front ? &s_frontSegment : &s_backSegment;
                const UnkSegment_80021DE4 &line = fn_80021DE4((const UnkSegment_80020A20 *)fn_80020A20(
                    &UnkSegment_80020A20(), &(const Vec3f &)fn_8040B67C(&matrix, skid->Start()),
                    &(const Vec3f &)fn_8040B67C(&matrix, skid->End())));
                fn_800091D0(&m_hangPosition,
                            &(const Vec3f &)fn_8040FC3C(line, &m_chara->m_position));
                Mtx34f inverse;
                fn_8040C148(&inverse, &(const Mtx34f &)fn_8040BEF8(&matrix));
                AssignVector(m_hangOffset, fn_8040B67C(&inverse, &m_hangPosition));
                HangCharacter(player);
                m_flags2.Set(1);
                return;
            }
        }
    }
}

void Helicopter::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB:
        if (m_flags2.Test(1)) {
            m_flags2.Clear(1);
            m_rider = static_cast<RideCommand &>(command).m_partner;
            m_frontHandle->TurnOff();
            m_backHandle->TurnOff();
            m_flags2.Set(2);
        }
        break;
    case 0xC:
        LetGo();
        break;
    }
    command.field_0x4[3] |= 2;
}

void Helicopter::LetGo() {
    m_flags2.Clear(2);
    m_flags2.Clear(1);
    m_frontHandle->TurnOn();
    m_backHandle->TurnOn();
    m_frontHandle->StartTimer(1.0f);
    m_backHandle->StartTimer(1.0f);
    m_rider.reset();
}

/* Not called anywhere: the linker strips it from the game. Retail pools IsZero's literal between
 * OnAttack's and LetGo's, which compiling an out-of-line function that inlines it at this point
 * of the file reproduces (as in ElecSmallCube.cpp; inferred). */
bool IsZeroOutOfLine(const Vec3f &vector) {
    return IsZero(vector);
}

void Helicopter::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock();
    if (attack.get() != 0 && (attack->m_flags & 2) == 0) {
        param.m_result = 1;
        if (!m_flags2.Test(0)) {
            m_health -= param.m_damage;
            if (m_health < 0.0f || fn_8000A950(m_health)) {
                param.m_result = 3;
                boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x39);
                }
                m_flags2.Set(0);
            }
        }
    }
}

extern "C" const char *fn_802F72A4(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_moveTypeLabels[2] = {"Straight", "Circle"};
    static const char *s_param0Labels[2] = {"MoveLengthX", "CircleRadius"};
    static const char *s_param1Labels[2] = {"MoveLengthY", "MoveSpd (point/sec)"};
    static const char *s_param2Labels[2] = {"MoveLengthZ", "InitPos (deg)"};
    static const char *s_moveSecLabels[2] = {"MoveSec", "Not Used"};
    static const char *s_pauseSecLabels[2] = {"PauseSec", "Not Used"};
    static int s_type = 0;

    if (cursor->index == 0) {
        int type = cursor->value;
        s_type = type;
        return s_moveTypeLabels[type];
    } else if (cursor->index == 1) {
        return s_param0Labels[s_type];
    } else if (cursor->index == 2) {
        return s_param1Labels[s_type];
    } else if (cursor->index == 3) {
        return s_param2Labels[s_type];
    } else if (cursor->index == 4) {
        return s_moveSecLabels[s_type];
    } else if (cursor->index == 5) {
        return s_pauseSecLabels[s_type];
    }
    return 0;
}

static GadgetParamDesc lbl_8055A3F0[] = {
    {2, "MoveType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_802F72A4, 0},
    {4, "MoveParam0", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802F72A4, 0},
    {4, "MoveParam1", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802F72A4, 0},
    {4, "MoveParam2", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_802F72A4, 0},
    {4, "MoveSec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_802F72A4, 0},
    {4, "PauseSec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_802F72A4, 0},
    {0},
};

GadgetClassDesc lbl_8055A508 = {
    "Helicopter", fn_802F8594, fn_802F8590, Helicopter::Instantiate, 0, 0, 0xFA2, 1, 0, 10,
    lbl_8055A3F0,
};
