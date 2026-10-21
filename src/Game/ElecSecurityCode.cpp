/* ElecSecurityCode (class name from RTTI): the security code an ELEC_SECURITY gadget
 * (ElecSecurityCodeGenerator) launches at the nearest character. A code grows out of the
 * generator for a second, spinning, and then homes in on the character. When an attack hits it, it
 * stops; a code of type 0 or 1 then splits into two codes of the next type, which fly off to either
 * side of the character, and a code of type 2 breaks into debris. Its model (SECURITY.BIN) has
 * three spinning frames per type and an attack collision. File name inferred from the class.
 * Method names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * Not yet matching (36/37 functions; all data matches, and the DOL differs only there): in
 * IsBlocking retail loads hit->m_collider before zeroing info ("lwz r3,8(r3); li r31,0"); GC/2.7
 * (and 3.0a3/3.0a5 on the preprocessed unit) emit the two in the other order for every source
 * shape tried (declaration orders, an inline accessor or AsCharInfoColli, a void* or reference
 * parameter, if/else, ternary, goto, comma expressions, optimisation pragmas). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/AngleUtil.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/ElecSecurityCode.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

struct RwObject {
    /* 0x0 */ u8 type;
    /* 0x1 */ u8 subKind;
    /* 0x2 */ u8 flags;
    /* 0x3 */ u8 privateFlags;
    /* 0x4 */ void *parent;
};

/* Only the members used here are declared (the frame is 0xA4 bytes). */
struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
};

/* What fn_8015FA78 passes for each part of a model (see ElecCoreProgram.cpp). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ RwObject *m_object;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

/* The search FindFrame serves: the index of the part wanted and its frame (names inferred). */
struct ElecSecurityCodeFrameSearch {
    /* 0x0 */ s32 index;
    /* 0x4 */ RwFrame *frame;
};

/* A model part found by fn_8015F7BC (only the name is used here; see Meteor.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

/* What fn_8026EE9C passes its filter for each collider it meets (layout from the filter; names
 * inferred). */
struct UnkSweepHit_8026EE9C {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_flags;
    /* 0x10 */ void *m_user; /* the user pointer passed to fn_8026EE9C */
};

/* The direction and speed an attack callback reports at 0x34 and 0x4C of AttackCallbackParam
 * (meanings inferred; see Meteor.cpp). */
struct ElecSecurityCodeHit {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ Vec3f direction;
    /* 0x40 */ u8 field_0x40[0x4C - 0x40];
    /* 0x4C */ float speed;
};

/* The fields of the game state used here (layout from its users; see FootingMovable.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xC8];
    /* 0xC8 */ u8 m_paused; /* nonzero while the game is paused (meaning inferred) */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp, Meteor.cpp
 * and ElecBarrier.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback, void *data);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp), and sets its
 * two hit callbacks (see TurnOver.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* Creates a target for owner, and releases one (see Spring.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Plays sound id at the character info (see Meteor.cpp and ElecBomb.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* The model loader (see BreakObj.cpp). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* RenderWare's RwFrameRotate about the y axis by angle (see ArkCannon.cpp), and RwFrameTransform
 * (combine 2: post-concatenate). */
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *transform, int combine);
/* Moves position by velocity, which acceleration changes, over dt (behaviour inferred). */
void fn_804103E8(Vec3f *position, Vec3f *velocity,
                 const Vec3f *acceleration, float scale, float dt);
/* Moves position from previous towards where velocity takes it, stopping at the colliders filter
 * accepts (behaviour inferred from its body; user is handed to the filter). */
bool fn_8026EE9C(Vec3f *position, const Vec3f *previous, Vec3f *velocity,
                 bool (*filter)(UnkSweepHit_8026EE9C *hit), void *user, float radius);
/* The character a character collider belongs to (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
/* Vector helpers (behaviour inferred from their bodies; see Meteor.cpp). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);     /* vector += value */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
Vec3f fn_8040CEE8(const Vec3f *vector); /* vector normalised */
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
/* Converts a direction into two angles (behaviour inferred). */
Vec2f fn_8040F760(const Vec3f &direction);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* vector += value (name inferred; see Bomb.cpp). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

inline Vec3f operator*(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

inline Vec3f operator+(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

inline Vec3f operator-(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* vector = value (name inferred). */
inline void SetVector(Vec3f &vector, const Vec3f &value) {
    vector.x = value.x;
    vector.y = value.y;
    vector.z = value.z;
}

/* The heights of the target above the code, by type. */
static const float lbl_804C1F8C[3] = {15.0f, 10.0f, 6.5f};

/* The name of the code's character info. */
static const char lbl_804C1F98[] = "ElecSecurityCode";

/* The homing turn rate and the spin of a growing code (radians per second). */
static const float lbl_805EF184 = 0.33333f * lbl_805FAD38;
static const float lbl_805EF188 = 6.0f * lbl_805FAD38;

/* Part callback: stores the frame of the part whose index is the one searched for, and stops the
 * walk once it has one (name inferred). */
static UnkModelPart_8015FA78 *FindFrame(UnkModelPart_8015FA78 *part, void *data) {
    ElecSecurityCodeFrameSearch *search = static_cast<ElecSecurityCodeFrameSearch *>(data);

    if (search != 0 && part->m_object != 0 && part->m_index == search->index) {
        search->frame = static_cast<RwFrame *>(part->m_object->parent);
        if (search->frame != 0) {
            return 0;
        }
    }
    return part;
}

ElecSecurityCode::ElecSecurityCode(int type, GadgetCharInfo *charInfo,
                                   ElecSecurityCodeGenerator *generator,
                                   const Vec2f &rotation, int kind)
    : m_type(type), m_angles(rotation), m_kind(kind) {
    m_time = 0.0f;
    m_state = 0;
    SetScale(0.0f);
    m_frameAngles[0] = m_frameAngles[1] = m_frameAngles[2] = 0.0f;
    m_chara = fn_8015CB88(0x57, 4, lbl_804C1F98);
    fn_800091D0(&m_chara->m_position, &charInfo->m_position);
    SetVector(m_chara->m_rotation, Vec3f(0.0f, 0.0f, 0.0f));
    m_model = new GadgetModelInfo(1, "SECURITY.BIN");
    m_model->fn_80162A68(m_type, 0);
    fn_80161FC8(m_model, m_type, m_chara);
    m_target = fn_801E7128(m_chara, 0x10C00);
    m_attack = fn_80161428(m_model, m_type);
    if (boost::shared_ptr<CharaColliAttack> attack = m_attack.lock()) {
        fn_80054DD0(attack.get(), 1, 1, 1.0f, boost::bind(&ElecSecurityCode::OnHit, this, _1),
                    boost::bind(&ElecSecurityCode::OnAttackEnd, this, _1));
        fn_80096B64(attack.get(), 1, boost::bind(&ElecSecurityCode::OnAttack, this, _1));
        attack->field_0x54 = 0;
        if (m_model != 0) {
            fn_80161230(m_model, m_type, -1);
        }
    }
    RebuildTransform();
    m_generator = generator;
    fn_801D54F4(fn_80053164(), 0x723A, m_chara.get(), 0, 1);
}

ElecSecurityCode::~ElecSecurityCode() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
    fn_801E7108(&m_target);
}

void ElecSecurityCodeList::push_back(ElecSecurityCode *const &code) {
    fn_8001D820(this, code);
}

bool ElecSecurityCode::Tick(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (lbl_8057D6A0.m_paused != 0) {
        return true;
    }

    switch (m_state) {
    case 0: {
        m_time += dt;
        float t = m_time;
        if (t >= 1.0f) {
            t = 1.0f;
            if (boost::shared_ptr<CharaColliAttack> attack = m_attack.lock()) {
                attack->field_0x54 = 1;
                if (m_model != 0) {
                    fn_8016111C(m_model, m_type, -1);
                }
            }
            m_state = 1;
        }
        m_scale.x = m_scale.z = t;
        m_scale.y = 2.0f * (1.0f - t) + 1.0f;
        m_chara->m_rotation.y += lbl_805EF188 * dt;
        if (m_kind != 0) {
            HomeSplit(dt);
        }
        break;
    }
    case 1:
        Home(dt);
        fn_801D53C4(fn_80053164(), 0x723B, m_chara.get(), this, 3, 0, 5);
        break;
    case 2:
        if (boost::shared_ptr<CharaColliAttack> attack = m_attack.lock()) {
            attack->field_0x54 = 0;
            if (m_model != 0) {
                fn_80161230(m_model, m_type, -1);
            }
        }
        fn_801D54F4(fn_80053164(), 0x7241, m_chara.get(), 0, 1);
        if (m_type == 0 || m_type == 1) {
            Vec3f direction;
            boost::shared_ptr<CharInfo> target =
                fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();

            if (target.get() != 0) {
                fn_800091D0(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                            &m_chara->m_position, &target->m_position));
                if (0.0f == direction.x && 0.0f == direction.z) {
                    fn_800091D0(&direction, &(const Vec3f &)fn_8040CEE8(
                                                &(const Vec3f &)Vec3f(1.0f, 0.0f, 0.5f)));
                } else {
                    fn_800091D0(&direction,
                                &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Vec3f(
                                    direction.z, 0.0f, direction.x)));
                    fn_800091D0(&direction, &(const Vec3f &)fn_8040D118(
                                                &direction, 0.125f * lbl_805FAD38));
                }
            } else {
                fn_800091D0(&direction, &(const Vec3f &)Vec3f(1.0f, 0.0f, 0.0f));
            }
            m_generator->m_codes.push_back(new ElecSecurityCode(
                m_type + 1,
                (GadgetCharInfo *)&(const Vec3f &)(m_chara.px->m_position + direction * 5.0f),
                m_generator, fn_8040F760(direction), 1));
            m_generator->m_codes.push_back(new ElecSecurityCode(
                m_type + 1,
                (GadgetCharInfo *)&(const Vec3f &)(m_chara.px->m_position - direction * 5.0f),
                m_generator, fn_8040F760(direction), 2));
            m_generator->field_0x3C = target;
            return false;
        } else if (m_type == 2) {
            Break();
            return false;
        }
        break;
    case 3:
        break;
    }

    RebuildTransform();
    UpdateFrames(dt);
    return true;
}

/* The sweep filter of Move: true for the colliders a code stops at, which are those of the other
 * characters that are not players (name inferred; the meanings of the flag and of the kinds 1, 2
 * and 6 are unknown). */
static bool IsBlocking(UnkSweepHit_8026EE9C *hit) {
    CharaColliBase *base = hit->m_collider;
    CharInfo *info = 0;

    if (CharaColliBaseCharInfo *collider = dynamic_cast<CharaColliBaseCharInfo *>(base)) {
        info = fn_800586EC(collider).lock().get();
    }
    if (info == 0) {
        return true;
    }
    if ((hit->m_flags & 0x100000) != 0 || info->field_0x5C == 1 || info->field_0x5C == 2 ||
        info->field_0x5C == 6 || hit->m_user == info) {
        return false;
    }
    return true;
}

void ElecSecurityCode::Home(float dt) {
    boost::shared_ptr<CharInfo> target = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                             &target->m_position, &m_chara->m_position));
    Vec2f angles = fn_8040F760(offset);

    m_angles.x = TurnAngleToward(angles.x, m_angles.x, lbl_805EF184);
    m_angles.y = TurnAngleToward(angles.y, m_angles.y, lbl_805EF184);
    Vec3f direction(0.0f, 0.0f, -1.0f);
    AssignVector(direction, fn_8040D118(&direction, m_angles.x));
    AssignVector(direction, fn_8040D0AC(&direction, m_angles.y));
    float distance = fn_8040CF2C(&offset);
    fn_800091D0(&m_chara->field_0x24, &target->field_0x24);
    if (distance > 0.0f) {
        AddVector(m_chara->field_0x24, fn_800091A4(&direction, 50.0f));
    }
    Move(dt);
}

void ElecSecurityCode::HomeSplit(float dt) {
    boost::shared_ptr<CharInfo> target = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                             &target->m_position, &m_chara->m_position));
    Vec2f angles = fn_8040F760(offset);

    switch (m_kind) {
    case 1:
        angles.y -= 0.85f * lbl_805FAD38;
        break;
    case 2:
        angles.y += 0.85f * lbl_805FAD38;
        break;
    }
    m_angles = angles;
    Vec3f direction(0.0f, 0.0f, -1.0f);
    AssignVector(direction, fn_8040D118(&direction, m_angles.x));
    AssignVector(direction, fn_8040D0AC(&direction, m_angles.y));
    float distance = fn_8040CF2C(&offset);
    fn_800091D0(&m_chara->field_0x24, &target->field_0x24);
    if (distance > 0.0f) {
        AddVector(m_chara->field_0x24, fn_800091A4(&direction, 75.0f));
    }
    Move(dt);
}

void ElecSecurityCode::Move(float dt) {
    Vec3f previous;
    fn_800091EC(&previous, &m_chara->m_position);
    fn_804103E8(&m_chara->m_position, &m_chara->field_0x18, &m_chara->field_0x24, 3.0f, dt);

    float height = 0.0f;
    switch (m_type) {
    case 0:
        height = 15.0f;
        break;
    case 1:
        height = 10.0f;
        break;
    case 2:
        height = 7.5f;
        break;
    }
    previous.y += height;
    m_chara->m_position.y += height;
    fn_8026EE9C(&m_chara->m_position, &previous, &m_chara->field_0x18, IsBlocking,
                m_chara.get(), height);
    m_chara->m_position.y -= height;
}

void ElecSecurityCode::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B720(&matrix, m_scale.x, m_scale.y, m_scale.z);
        m_model->fn_801633DC(&matrix);
    }
    if (m_target.get() != 0) {
        Vec3f position;

        fn_800091EC(&position, &m_chara->m_position);
        position.y += lbl_804C1F8C[m_type];
        m_target->fn_800CCE38(&position);
    }
}

void ElecSecurityCode::UpdateFrames(float dt) {
    if (m_model != 0) {
        ElecSecurityCodeFrameSearch search;

        search.index = 0;
        search.frame = 0;
        for (int i = 0; i < 3; i++) {
            search.index = i + m_type * 3;
            search.frame = 0;
            fn_8015FA78(m_model, FindFrame, &search);
            m_frames[i] = search.frame;
            if (search.frame != 0) {
                m_frameMatrices[i] = search.frame->modelling;
            }
        }
        if (m_frames[0] != 0) {
            m_frameAngles[0] += lbl_805FAD38 * dt;
            fn_800E9CEC(m_frames[0], m_frameAngles[0], 0);
            fn_804876E8(m_frames[0], &m_frameMatrices[0], 2);
        }
        if (m_frames[1] != 0) {
            m_frameAngles[1] += 2.0f * lbl_805FAD38 * dt;
            fn_800E9CEC(m_frames[1], m_frameAngles[1], 0);
            fn_804876E8(m_frames[1], &m_frameMatrices[1], 2);
        }
        if (m_frames[2] != 0) {
            m_frameAngles[2] += -2.0f * lbl_805FAD38 * dt;
            fn_800E9CEC(m_frames[2], m_frameAngles[2], 0);
            fn_804876E8(m_frames[2], &m_frameMatrices[2], 2);
        }
    }
}

void ElecSecurityCode::Break() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 9);
        if (part != 0) {
            void *loader = fn_800122D4();
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.scale = 1.3f;
            debris.model = fn_80012700(loader, 1, part->name);
            debris.field_0x54 = 1;
            debris.field_0x4C = 150;
            fn_8015D93C(&debris);
        }
    }
}

void ElecSecurityCode::OnHit(AttackCallbackParam &param) {
    ElecSecurityCodeHit *hit = reinterpret_cast<ElecSecurityCodeHit *>(&param);

    fn_800091D0(&hit->direction, &m_chara->field_0x18);
    hit->speed = fn_8040CF50(&hit->direction);
    fn_8040CE74(&hit->direction);
    if (0.0f == hit->speed) {
        Vec3f up(0.0f, 1.0f, 0.0f);
        fn_800091D0(&hit->direction, &up);
        hit->speed = 1.0f;
    }
}

void ElecSecurityCode::OnAttackEnd(AttackCallbackParam &param) {}

void ElecSecurityCode::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        param.m_result = 3;
        m_state = 2;
    } else {
        param.m_result = 5;
    }
}
