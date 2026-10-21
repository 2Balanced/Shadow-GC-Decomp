/* Meteor (class name from RTTI): the METEOR_L gadget, a large meteor (METEOR.BIN) in space. It
 * waits until the nearest character comes within "RANGE m", then flies along the direction given
 * by "DIRECTION_X deg" and "DIRECTION_Y deg" at "MOVE_SPD m/s", spinning, for "LIFETIME s"; in its
 * last second its colour fades from red towards yellow. One hit breaks it: it rewards the
 * attacker, breaks into its debris, spawns three small meteors (METEORS.BIN) from its model's
 * nodes 1 to 3 and finishes. Its attack collision also hits what it flies into. File name inferred
 * from the class. Method names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
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

extern "C" {
/* Builds the character info of a gadget type (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* The sound manager (see GadgetUtil.h). */
void *fn_80053164(void);
/* A debug line from start to end (see GadgetUtil.h; segment is 0x18 bytes of caller stack). */
void *fn_80020A20(UnkDebugLine_80020A20 *segment, const Vec3f *start, const Vec3f *end);
/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp and
 * ElecBarrier.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80161A24(GadgetModelInfo *model, int arg1);
void fn_80162790(GadgetModelInfo *model);
int fn_80162424(GadgetModelInfo *model, int id);
UnkMaterialAnim_801621F0 *fn_80162298(GadgetModelInfo *model, int id);
/* Finds the matrix of node id of a model (see ArkCannon.cpp). */
bool fn_80169EF0(Mtx34f *matrix, boost::shared_ptr<GadgetModelInfo> model, int arg2, int id);
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

/* Vector helpers (behaviour inferred from their bodies; see ArkCannon.cpp and Bomb.cpp). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CDD0(Vec3f *result, const Vec3f *a, const Vec3f *b); /* a + b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);     /* vector += value */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
/* The translation of a matrix, and the Euler angles of a rotation matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* Draws a debug line set up by fn_80020A20 in color (behaviour inferred). */
void fn_802E0AA0(UnkDebugLine_80020A20 *segment, const UnkColor_8040D2D8 &color, float width);
}

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h, whose non-const
 * declaration of pi this unit cannot use). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* fn_8040CDA8 called with its result pointer spelled out: ReloadPlacement keeps every vector in a
 * named local, which gives retail's stack slots and argument order. */
typedef void (*VectorScaleIntoFn)(Vec3f *result, const Vec3f *vector, float scale);

/* vector += value (name inferred; see Bomb.cpp). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The parameters of the owner record (names from the parameter table). */
struct MeteorParams {
    /* 0x00 */ float range;     /* "RANGE m" */
    /* 0x04 */ float speed;     /* "MOVE_SPD m/s" */
    /* 0x08 */ float directionX; /* "DIRECTION_X deg" */
    /* 0x0C */ float directionY; /* "DIRECTION_Y deg" */
    /* 0x10 */ float spinX;      /* "ROT_SPD_X deg/s" */
    /* 0x14 */ float spinZ;      /* "ROT_SPD_Z deg/s" */
    /* 0x18 */ float spinY;      /* "ROT_SPD_Y deg/s" */
    /* 0x1C */ float lifeTime;   /* "LIFETIME s" */
};
SIZE_ASSERT(MeteorParams, 0x20);

class Meteor : public Gadget {
  public:
    Meteor(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Meteor();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Moves the model and the target to the character info's place (name inferred). */
    void RebuildTransform();
    /* The callbacks of the model's attack collision (names inferred). */
    void OnAttackEnd(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ MeteorParams m_params;
    /* 0x50 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x58 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x60 */ boost::shared_ptr<Target> m_target;
    /* 0x68 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x70 */ s32 m_state; /* 0 waiting, 1 flying, 2 broken */
    /* 0x74 */ Vec3f m_direction;
    /* 0x80 */ float m_lifeTime;
    /* 0x84 */ float m_health;
};
SIZE_ASSERT(Meteor, 0x88);

const char *Meteor::s_taskName = "Meteor";

/* Converts the parameters' metres to world units (name inferred). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* The model nodes the small meteors start from. */
static const int lbl_804F05E8[] = {1, 2, 3};

/* Returns x unchanged (name inferred). Being initialised through a function call, the value below
 * is set up at startup (see TurnOver.cpp). */
inline float Plain(float x) {
    return x;
}

/* The colours the meteor fades between in its last second (meaning inferred). */
static UnkColor_8040D2D8 lbl_805EFD54(0xFF, 0x4B, 0x1B, 0x1B);
static UnkColor_8040D2D8 lbl_805EFD58(0xFF, 0xFF, 0x73, 0x00);
/* Passed to the small meteors' motion (meaning unknown). A constant with a dynamic initializer,
 * so it lives in .sbss and the static initializer stores it (as in ArkCannon.cpp). */
static const float lbl_805EFD5C = Plain(100.0f);

Meteor::Meteor(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner), m_state(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x61, 4);
    ReadPlacement(owner);
    m_target = fn_801E72DC(0xA800);
    m_model = fn_8015F800(1, "METEOR.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80162424(m_model.get(), 0);
    UnkMaterialAnim_801621F0 *anim = fn_80162298(m_model.get(), 0);
    anim->color = lbl_805EFD54;
    RebuildTransform();
    m_particle = fn_80166AF4();
    {
        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_particle;
        fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        fn_80167010(m_particle.get(), -1);
    }
    fn_80161FC8(m_model.get(), 0, m_chara);
    boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
    if (collision.get() != 0) {
        fn_80096B64(collision.get(), 1, boost::bind(&Meteor::OnAttack, this, _1));
        fn_80054DD0(collision.get(), 4, 1, 5.0f, boost::bind(&Meteor::OnHit, this, _1),
                    boost::bind(&Meteor::OnAttackEnd, this, _1));
    }
}

Meteor::~Meteor() {}

extern "C" void fn_8021D330(void) {
    fn_8006B474("METEOR.BIN", 1);
    fn_8006B474("METEORS.BIN", 1);
}

extern "C" void fn_8021D32C(void) {}

void Meteor::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Meteor(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void Meteor::Behave(float dt) {
    switch (m_state) {
    case 0: {
        boost::shared_ptr<CharInfo> enemy = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
        if (enemy.get() != 0) {
            Vec3f offset;

            fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                     &enemy->m_position, &m_chara->m_position));
            if (fn_8040CF50(&offset) < m_params.range) {
                m_state = 1;
            }
        }
        break;
    }
    case 1:
    {
        const Vec3f &velocity = fn_8040CDA8(&m_direction, m_params.speed * dt);
        fn_800091D0(&m_chara->field_0x18, &velocity);
    }
        AddVector(m_chara->m_position, m_chara->field_0x18);
        m_lifeTime -= dt;
        if (m_lifeTime < 0.0f) {
            m_state = 2;
        } else if (m_lifeTime < 1.0f) {
            float t = 1.0f - m_lifeTime;
            float g = t * (lbl_805EFD58.g - lbl_805EFD54.g);
            float b = t * (lbl_805EFD58.b - lbl_805EFD54.b);
            float a = t * (lbl_805EFD58.a - lbl_805EFD54.a);
            g += lbl_805EFD54.g;
            g = fn_80070460(g, 0.0f, 255.0f);
            b += lbl_805EFD54.b;
            b = fn_80070460(b, 0.0f, 255.0f);
            a += lbl_805EFD54.a;
            a = fn_80070460(a, 0.0f, 255.0f);
            UnkMaterialAnim_801621F0 *anim = fn_80162298(m_model.get(), 0);
            anim->color.Set(0xFF, g, b, a);
        }
        break;
    case 2: {
        fn_80161A24(m_model.get(), 0);
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        fn_80162790(m_model.get());
        fn_801D54F4(fn_80053164(), 0x772F, m_chara.get(), 0, 1);
        for (u32 i = 0; i < 3; i++) {
            Mtx34f matrix;
            if (fn_80169EF0(&matrix, m_model, 0, lbl_804F05E8[i])) {
                Vec3f position;
                fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(&matrix));
                Vec3f direction;
                fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                            &position, &m_chara->m_position));
                fn_8040CE74(&direction);
                Vec3f rotation(0.0f, 0.0f, 0.0f);
                fn_800494F8(&rotation, &matrix);
                Vec3f spin(m_params.spinX, m_params.spinY, m_params.spinZ);
                UnkMeteorMotion_8021FCE4 motion(direction, spin, 0.0f, lbl_805EFD5C, 3.0f);
                new MeteorsTask(fn_801EB304(fn_8001C4F8(), 11), &position, &rotation, &motion, 1);
            }
        }
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), 1);
        debris.model = fn_80012700(fn_800122D4(), 1, part->name);
        debris.field_0x40 = m_chara->m_position.y;
        debris.scale = 1.0f;
        debris.field_0x4A = 0;
        debris.field_0x54 = 1;
        fn_8015D93C(&debris);
        fn_800C9AD4(m_owner);
        m_flags |= 1;
        return;
    }
    }

    m_chara->m_rotation.x += m_params.spinX * dt;
    m_chara->m_rotation.y += m_params.spinY * dt;
    m_chara->m_rotation.z += m_params.spinZ * dt;
    RebuildTransform();
    m_model->fn_80162FB0(dt);
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
}

void Meteor::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.range);

    float length = m_params.lifeTime * m_params.speed;
    if (length < 100.0f) {
        length = 100.0f;
    }
    UnkDebugLine_80020A20 segment;
    Vec3f offset;
    Vec3f scaled;
    Vec3f end;
    ((VectorScaleIntoFn)fn_8040CDA8)(&scaled, &m_direction, length);
    fn_800091EC(&offset, &scaled);
    fn_8040CDD0(&end, &m_chara->m_position, &offset);
    fn_80020A20(&segment, &m_chara->m_position, &end);
    fn_802E0AA0(&segment, UnkColor_8040D2D8(0xFF, 0xFF, 0xFF, 0x00), 10.0f);
}

void Meteor::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.range = MetresToUnits(m_params.range);
    m_params.speed = MetresToUnits(m_params.speed);
    m_params.directionX *= lbl_805FAD38 / 180.0f;
    m_params.directionY *= lbl_805FAD38 / 180.0f;
    m_params.spinX *= lbl_805FAD38 / 180.0f;
    m_params.spinZ *= lbl_805FAD38 / 180.0f;
    m_params.spinY *= lbl_805FAD38 / 180.0f;
    m_lifeTime = m_params.lifeTime;
    Vec3f forward;
    forward.x = 0.0f;
    forward.y = 0.0f;
    forward.z = 1.0f;
    AssignVector(m_direction, forward);
    AssignVector(m_direction, fn_8040D118(&m_direction, m_params.directionX));
    AssignVector(m_direction, fn_8040D0AC(&m_direction, m_params.directionY));
    m_state = 0;
    m_health = 1.0f;
}

void Meteor::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
}

void Meteor::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), m_direction);
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = m_params.speed;
    param.m_damage = 5.0f;
}

void Meteor::OnAttackEnd(AttackCallbackParam &param) {}

void Meteor::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (m_state != 2) {
        m_health -= param.m_damage;
        if (m_health < 0.0f || fn_8000A950(m_health)) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x4A);
            }
            m_state = 2;
        }
    }
}

static GadgetParamDesc lbl_80545A64[] = {
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

GadgetClassDesc lbl_80545BCC = {
    "METEOR_L", fn_8021D330, fn_8021D32C, Meteor::Instantiate, 0, 0, 0x138A, 1, 0, 10, lbl_80545A64,
};
