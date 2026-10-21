/* MeteorsHolder (class name from RTTI): the METEOR_S gadget, a small meteor (METEORS.BIN) in space.
 * The gadget holds one Meteors (class name from the RTTI of its shared_ptr control block), which
 * waits until the nearest character comes within "RANGE m", then flies along the direction given
 * by "DIRECTION_X deg" and "DIRECTION_Y deg" at "MOVE_SPD m/s", spinning, for "LIFETIME s"; in its
 * last second its colour fades from red towards yellow. It breaks when it is hit, when its life
 * runs out or when it flies into something: it rewards the attacker, breaks into its debris and
 * finishes the task that runs it. MeteorsTask (class name from RTTI) runs the small meteors the
 * large meteor (METEOR_L, Meteor.cpp) spawns. File name inferred from the gadget class. Method
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Meteors.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The orphan RTTI base lists that start this unit's .data (see docs/matching-gadgets.md). */
#include "Game/TNumEdit.h"

#include "Game/CharaColliBaseCharInfo.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* CollisionResultList (class name from the RTTI of its shared_ptr control block): the hits a
 * collision query returns (see ScatteringRing.cpp, which releases one through fn_80026FB4). */
class CollisionResultList;
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResultList;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* Four colour bytes built by an out-of-line constructor (see ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* Sets the four bytes (name inferred). */
    void Set(u8 r_, u8 g_, u8 b_, u8 a_) {
        r = r_;
        g = g_;
        b = b_;
        a = a_;
    }

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation fn_80162298 returns (see ElecCoreProgram.cpp). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

/* A debug line segment set up by fn_80020A20 (0x18 bytes; contents unknown). */
struct UnkDebugLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A model part found by fn_8015F7BC (only the name is used here). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

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

/* A node of a collider's contact list (see BlockChange.cpp): the contact's collider at 0xC, whose
 * attribute bits are at 0x1C (names inferred). */
struct UnkColliContactCollider_8021E520 {
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ u32 m_attributes;
};
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ UnkColliContactCollider_8021E520 *m_collider;
};

/* An iterator over a contact list (layout from fn_8001EDFC; see BlockChange.cpp). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* What fn_80021934 passes its filter for each candidate hit (layout from the filter; names
 * inferred). */
struct UnkCollisionCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ CharaColliBase *m_self;
};

/* A sphere: centre and radius (built by fn_80021C04). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

extern "C" {
/* Builds the character info of a gadget type (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* The sound manager (see GadgetUtil.h). */
void *fn_80053164(void);
/* A debug line from start to end (see GadgetUtil.h; segment is 0x18 bytes of caller stack). */
void *fn_80020A20(UnkDebugLine_80020A20 *segment, const Vec3f *start, const Vec3f *end);
/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp, Door.cpp and
 * ElecBarrier.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
void fn_80161A24(GadgetModelInfo *model, int arg1);
void fn_80162790(GadgetModelInfo *model);
int fn_80162424(GadgetModelInfo *model, int id);
UnkMaterialAnim_801621F0 *fn_80162298(GadgetModelInfo *model, int id);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp), and sets its
 * two hit callbacks (see TurnOver.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* Loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Creates a target with the given flags (see CircusBall.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Whether value is zero within a small tolerance (from its body). */
bool fn_8000A950(float value);
/* value clamped between low and high (behaviour inferred from its body). */
const float &fn_80070460(const float &value, const float &low, const float &high);
/* Plays sound id at the character info (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* Shakes the camera around a character info (behaviour inferred; see CityBombBig.cpp). */
void fn_801DD0BC(GadgetCharInfo *charInfo, int arg1, float arg2, int arg3);
/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);

/* Particle controller helpers (see TurnOver.cpp; fn_80166F88 stops the controller, inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
/* Calls callback with each part of the model; fn_80169D00 attaches a particle controller to a part
 * (see TurnOver.cpp). */
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The contact list: its first and end nodes and the iterator post-increment (see
 * BlockChange.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);

/* The collision world (see ScatteringRing.cpp), a sphere from a centre and a radius, and the sweep
 * of a sphere through the world along velocity; it returns the hits that filter accepts, or null
 * (behaviour inferred). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere, const Vec3f *velocity,
                                 int arg3, int arg4, int arg5,
                                 bool (*filter)(UnkCollisionCandidate_80021934 *candidate),
                                 CharaColliBase *self, int arg8);
/* Releases a collision result list (see ScatteringRing.cpp). */
void fn_80026FB4(CollisionResultList *list, int arg1);

/* Compares two blocks of memory (behaviour inferred). */
int fn_803A8DA8(const void *a, const void *b, u32 size);

/* Vector helpers (behaviour inferred from their bodies; see ArkCannon.cpp and Bomb.cpp). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CDD0(Vec3f *result, const Vec3f *a, const Vec3f *b); /* a + b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);     /* vector += value */
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
/* Draws a debug line set up by fn_80020A20 in color (behaviour inferred). */
void fn_802E0AA0(UnkDebugLine_80020A20 *segment, const UnkColor_8040D2D8 &color, float width);
}

/* Retail releases a collision result list through its out-of-line release (fn_80026FB4, see
 * ScatteringRing.cpp) rather than a declared destructor. */
namespace boost {
template<> inline void checked_delete<CollisionResultList>(CollisionResultList *x) {
    fn_80026FB4(x, 1);
}
} // namespace boost

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* fn_8040CDA8 called with its result pointer spelled out (see Meteor.cpp). */
typedef void (*VectorScaleIntoFn)(Vec3f *result, const Vec3f *vector, float scale);

/* vector += value (name inferred; see Bomb.cpp). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The flag word at 0x2C (struct name unknown). */
struct MeteorsFlags {
    MeteorsFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
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

    u32 m_bits; /* bit 0 hit this frame, bit 1 breaking */
};

class Meteors {
  public:
    Meteors(Task *task, const Vec3f *position, const Vec3f *rotation,
            const UnkMeteorMotion_8021FCE4 *motion, int state);
    ~Meteors();

    /* Per-frame update (name inferred). */
    void Tick(float dt);
    /* Moves back to the owner's placement (name inferred). */
    void Reset(UnkPlacement_800C9CD0 *owner);
    /* Moves the model and the target to the character info's place (name inferred). */
    void RebuildTransform();
    /* The model's collision callback and the callbacks of its attack collision (names
     * inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    void OnAttack(AttackCallbackParam &param);

    /* Member names are inferred. */
    /* 0x00 */ Task *m_task;   /* the task that runs the meteor */
    /* 0x04 */ s32 m_state;    /* 0 waiting, 1 flying, 2 broken */
    /* 0x08 */ UnkMeteorMotion_8021FCE4 m_motion;
    /* 0x2C */ MeteorsFlags m_flags;
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<Target> m_target;
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x48 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x50 */ float m_lifeTime;
    /* 0x54 */ float m_health;
};
SIZE_ASSERT(Meteors, 0x58);

/* Deletes a meteor (name inferred; see checked_delete below). */
inline void DeleteMeteors(Meteors *meteors) {
    delete meteors;
}

/* Retail keeps boost::checked_delete<Meteors> out of line and ~Meteors at its own place (see
 * docs/research-notes.md and Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<Meteors>(Meteors *x) {
    DeleteMeteors(x);
}
} // namespace boost

class MeteorsHolder : public Gadget {
  public:
    MeteorsHolder(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~MeteorsHolder();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Creates a new meteor at the owner's placement (name inferred). */
    void Spawn(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<Meteors> m_meteors;
    /* 0x38 */ MeteorsParams m_params;
};
SIZE_ASSERT(MeteorsHolder, 0x58);

/* The radius of the sphere a flying meteor sweeps for obstacles (meaning inferred). Being
 * initialised through a function call, the value is set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}
static const float lbl_805EFD74 = Plain(25.0f);

/* The colours the meteor fades between in its last second (meaning inferred). */
static UnkColor_8040D2D8 lbl_805EFD78(0xFF, 0x4B, 0x1B, 0x1B);
static UnkColor_8040D2D8 lbl_805EFD7C(0xFF, 0xFF, 0x73, 0x00);

/* Converts the parameters' metres to world units (name inferred). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* Accepts the hits of the obstacle sweep: not the meteor's own collider, and none with attribute
 * bits 0x40018 (name and meaning inferred). */
static bool AcceptObstacle(UnkCollisionCandidate_80021934 *candidate) {
    if (candidate->m_collider != 0 && candidate->m_collider == candidate->m_self) {
        return false;
    }
    if (candidate->m_attributes & 0x40018) {
        return false;
    }
    return true;
}

UnkMeteorMotion_8021FCE4::UnkMeteorMotion_8021FCE4(const MeteorsParams &params) : spin(0.0f, 0.0f, 0.0f) {
    range = MetresToUnits(params.range);
    speed = MetresToUnits(params.speed);
    spin.x = params.spinX * (lbl_805FAD38 / 180.0f);
    spin.z = params.spinZ * (lbl_805FAD38 / 180.0f);
    spin.y = params.spinY * (lbl_805FAD38 / 180.0f);
    Vec3f forward(0.0f, 0.0f, 1.0f);
    AssignVector(direction, forward);
    AssignVector(direction, fn_8040D118(&direction, params.directionX * (lbl_805FAD38 / 180.0f)));
    AssignVector(direction, fn_8040D0AC(&direction, params.directionY * (lbl_805FAD38 / 180.0f)));
    lifeTime = params.lifeTime;
}

UnkMeteorMotion_8021FCE4::UnkMeteorMotion_8021FCE4(const Vec3f &direction_, Vec3f spin_,
                                                   float range_, float speed_, float lifeTime_)
    : range(range_), speed(speed_) {
    fn_800091EC(&direction, &direction_);
    spin = spin_;
    lifeTime = lifeTime_;
}

UnkMeteorMotion_8021FCE4::UnkMeteorMotion_8021FCE4(const UnkMeteorMotion_8021FCE4 &other)
    : range(other.range), speed(other.speed) {
    fn_800091EC(&direction, &other.direction);
    spin = other.spin;
    lifeTime = other.lifeTime;
}

Meteors::Meteors(Task *task, const Vec3f *position, const Vec3f *rotation,
                 const UnkMeteorMotion_8021FCE4 *motion, int state)
    : m_task(task), m_state(state), m_motion(*motion), m_flags(0) {
    m_lifeTime = motion->lifeTime;
    m_health = 1.0f;
    m_flags.Set(0);
    m_chara = fn_8015CD90(0x62, 4);
    fn_800091D0(&m_chara.get()->m_position, position);
    m_chara->m_rotation = *rotation;
    m_target = fn_801E72DC(0xA800);
    m_model = fn_8015F800(1, "METEORS.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80162424(m_model.get(), 0);
    UnkMaterialAnim_801621F0 *anim = fn_80162298(m_model.get(), 0);
    anim->color = lbl_805EFD78;
    RebuildTransform();
    m_particle = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_particle;
        fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        fn_80167010(m_particle.get(), -1);
    }
    fn_80161FC8(m_model.get(), 0, m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&Meteors::OnColliDetect, this, _1));
    {
        boost::shared_ptr<CharaColliBase> collider = fn_80161614(m_model.get(), 0).lock();
        if (collider.get() != 0) {
            collider->SetAttributes(0x100100, -1);
        }
    }
    fn_80161230(m_model.get(), 0, -1);
    boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
    if (collision.get() != 0) {
        fn_80096B64(collision.get(), 4, boost::bind(&Meteors::OnAttack, this, _1));
        fn_80054DD0(collision.get(), 1, 1, 3.0f, boost::bind(&Meteors::OnHit, this, _1),
                    boost::bind(&Meteors::OnAttackEnd, this, _1));
    }
}

Meteors::~Meteors() {}

void Meteors::Tick(float dt) {
    switch (m_state) {
    case 0: {
        boost::shared_ptr<CharInfo> enemy = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
        if (enemy.get() != 0) {
            Vec3f offset;

            fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                     &enemy->m_position, &m_chara->m_position));
            if (fn_8040CF50(&offset) < m_motion.range) {
                m_state = 1;
            }
        }
        break;
    }
    case 1:
    {
        const Vec3f &velocity = fn_8040CDA8(&m_motion.direction, m_motion.speed * dt);
        fn_800091D0(&m_chara->field_0x18, &velocity);
    }
        AddVector(m_chara->m_position, m_chara->field_0x18);
        m_lifeTime -= dt;
        if (m_lifeTime < 0.0f) {
            m_flags.Set(1);
        } else if (m_lifeTime < 1.0f) {
            float t = 1.0f - m_lifeTime;
            float g = t * (lbl_805EFD7C.g - lbl_805EFD78.g);
            float b = t * (lbl_805EFD7C.b - lbl_805EFD78.b);
            float a = t * (lbl_805EFD7C.a - lbl_805EFD78.a);
            g += lbl_805EFD78.g;
            g = fn_80070460(g, 0.0f, 255.0f);
            b += lbl_805EFD78.b;
            b = fn_80070460(b, 0.0f, 255.0f);
            a += lbl_805EFD78.a;
            a = fn_80070460(a, 0.0f, 255.0f);
            UnkMaterialAnim_801621F0 *anim = fn_80162298(m_model.get(), 0);
            anim->color.Set(0xFF, g, b, a);
        }
        break;
    }

    if (m_state != 2) {
        if (!m_flags.Test(0)) {
            boost::shared_ptr<CharaColliBase> collider = fn_80161614(m_model.get(), 0).lock();
            if (collider.get() != 0) {
                UnkSphere_80021C04 sphere;
                fn_80021C04(&sphere, &m_chara.get()->m_position, lbl_805EFD74);
                Vec3f velocity;
                fn_800091EC(&velocity, &m_chara.get()->field_0x18);
                boost::shared_ptr<CollisionResultList> hits(
                    fn_80021934(fn_80020D40(), &sphere, &velocity, 3, 0xF, 0, AcceptObstacle, collider.get(), 0));
                if (hits.get() != 0) {
                    m_flags.Set(1);
                }
            }
        }
        m_chara.get()->m_rotation.x += m_motion.spin.x * dt;
        m_chara.get()->m_rotation.y += m_motion.spin.y * dt;
        m_chara.get()->m_rotation.z += m_motion.spin.z * dt;
        RebuildTransform();
        m_model->fn_80162FB0(dt);
        if (m_particle.get() != 0) {
            fn_8016761C(m_particle.get(), dt);
        }
    }

    if (m_flags.Test(0)) {
        fn_8016111C(m_model.get(), 0, -1);
        m_flags.Clear(0);
    }
    if (m_flags.Test(1)) {
        fn_80161A24(m_model.get(), 0);
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        fn_80162790(m_model.get());
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), 1);
        debris.model = fn_80012700(fn_800122D4(), 1, part->name);
        debris.field_0x40 = m_chara->m_position.y;
        debris.scale = 1.0f;
        debris.field_0x4A = 0;
        debris.field_0x54 = 1;
        fn_8015D93C(&debris);
        fn_801DD0BC(m_chara.get(), 1, 3.0f, 7);
        fn_801D54F4(fn_80053164(), 0x772F, m_chara.get(), 0, 1);
        m_task->m_flags |= 1;
        m_state = 2;
        m_flags.Clear(1);
    }
}

void Meteors::Reset(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara.get()->m_position);
    fn_800C9ED4(owner, &m_chara.get()->m_rotation);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_motion.range);

    float length = m_motion.lifeTime * m_motion.speed;
    if (length < 50.0f) {
        length = 50.0f;
    }
    UnkDebugLine_80020A20 segment;
    Vec3f offset;
    Vec3f scaled;
    Vec3f end;
    ((VectorScaleIntoFn)fn_8040CDA8)(&scaled, &m_motion.direction, length);
    fn_800091EC(&offset, &scaled);
    fn_8040CDD0(&end, &m_chara->m_position, &offset);
    fn_80020A20(&segment, &m_chara->m_position, &end);
    fn_802E0AA0(&segment, UnkColor_8040D2D8(0xFF, 0xFF, 0xFF, 0x00), 10.0f);
}

void Meteors::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
}

void Meteors::OnColliDetect(ColliDetectArgs &args) {
    if (m_state != 2 && args.m_contacts != 0) {
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(args.m_contacts)); it.m_node != fn_8001EDEC(args.m_contacts); fn_80027374(&it, 0)) {
            if ((it.m_node->m_collider->m_attributes & 0x40018) == 0) {
                m_flags.Set(1);
                return;
            }
        }
    }
}

void Meteors::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), m_motion.direction);
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = m_motion.speed;
    param.m_damage = 3.0f;
}

void Meteors::OnAttackEnd(AttackCallbackParam &param) {
    if (param.m_result == 1) {
        m_flags.Set(1);
    }
}

void Meteors::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (m_state != 2) {
        m_health -= param.m_damage;
        if (m_health < 0.0f || fn_8000A950(m_health)) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x49);
            }
            m_flags.Set(1);
        }
    }
}

const char *MeteorsTask::s_taskName = "MeteorsTask";

MeteorsTask::MeteorsTask(Task *parent, const Vec3f *position, const Vec3f *rotation,
                         const UnkMeteorMotion_8021FCE4 *motion, int state)
    : Task(parent) {
    m_name = s_taskName;
    m_meteors = boost::shared_ptr<Meteors>(new Meteors(this, position, rotation, motion, state));
}

MeteorsTask::~MeteorsTask() {}

void MeteorsTask::Tick(float dt) {
    if (m_meteors.get() != 0) {
        m_meteors->Tick(dt);
    }
}

const char *MeteorsHolder::s_taskName = "MeteorsHolder";

MeteorsHolder::MeteorsHolder(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

MeteorsHolder::~MeteorsHolder() {}

extern "C" void fn_8021E19C(void) {
    fn_8006B474("METEORS.BIN", 1);
}

extern "C" void fn_8021E198(void) {}

void MeteorsHolder::Instantiate(UnkPlacement_800C9CD0 *owner) {
    MeteorsHolder *holder = new MeteorsHolder(fn_801EB304(fn_8001C4F8(), 11), owner);
    holder->Spawn(owner);
    fn_800C9AD4(owner);
}

void MeteorsHolder::Behave(float dt) {
    if (m_meteors.get() != 0) {
        m_meteors->Tick(dt);
    }
}

void MeteorsHolder::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    MeteorsParams previous = m_params;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (fn_803A8DA8(&previous, &m_params, sizeof(m_params)) != 0) {
        Spawn(owner);
    }
    if (m_meteors.get() != 0) {
        m_meteors->Reset(owner);
    }
}

void MeteorsHolder::Spawn(UnkPlacement_800C9CD0 *owner) {
    Vec3f position;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_800C9F14(owner, &position);
    fn_800C9ED4(owner, &rotation);
    UnkMeteorMotion_8021FCE4 motion(m_params);
    m_meteors = boost::shared_ptr<Meteors>(new Meteors(this, &position, &rotation, &motion, 0));
}

static GadgetParamDesc lbl_80545CD8[] = {
    {4, "RANGE m", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MOVE_SPD m/s", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "DIRECTION_X deg", 0, 0, 0, 0.0f, -180.0f, 180.0f, fn_800CBC20, 0},
    {4, "DIRECTION_Y deg", 0, 0, 0, 0.0f, -180.0f, 180.0f, fn_800CBC20, 0},
    {4, "ROT_SPD_X deg/s", 0, 0, 0, 0.0f, -1080.0f, 1080.0f, fn_800CBC20, 0},
    {4, "ROT_SPD_Z deg/s", 0, 0, 0, 0.0f, -1080.0f, 1080.0f, fn_800CBC20, 0},
    {4, "ROT_SPD_Y deg/s", 0, 0, 0, 0.0f, -1080.0f, 1080.0f, fn_800CBC20, 0},
    {4, "LIFETIME s", 0, 0, 0, 5.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80545E40 = {
    "METEOR_S", fn_8021E19C, fn_8021E198, MeteorsHolder::Instantiate, 0, 0, 0x138B, 1, 0, 10, lbl_80545CD8,
};
