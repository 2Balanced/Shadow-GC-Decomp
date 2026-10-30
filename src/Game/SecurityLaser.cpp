/* SecurityLaser::SetSecurityLaser (class name from RTTI): the SECURITY LASER gadget, a laser emitter
 * (SECURITYLASER.BIN, variant "model") whose beam (variants 2 and 3) leaves a frame of the model and
 * reaches "length" along it. The beam can sweep "rot Y(deg)" to each side every "rot time(sec)"
 * seconds, and the emitter can swing along the offset ("X", "Y", "Z") every "move sec" seconds. The
 * beam carries an attack collision. Of the three "type"s, "Normal" stops the beam at what it meets and
 * shows an effect where it ends, "Fixed" keeps its full length without the effect and "Fixed w/Eff"
 * keeps its full length with the effect. File name inferred from the namespace. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The unit's code reaches its .data through a pooled base address, so
 * the linker keeps that section whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the
 * RTTI base lists it starts with. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBullet.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

#include <stdio.h>

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Weapon::Bullet::HitResult;

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in Rocket.cpp). */
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
SIZE_ASSERT(RwMatrix, 0x40);

struct RwFrame;

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (as in Rocket.cpp). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

/* A model's table of named entries in its resource file, and its sound entry (as in Door.cpp). */
class UnkModelTable_80208888;

struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 soundId; /* passed to the sound calls as 16 bits */
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

/* What the probe along the beam hits (class name unknown; as in ContainerBehavior.cpp). */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ void *m_surface;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object;
};

/* Passed to a parameter's label function (as in BAGunShip.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* Degrees to radians: an out-of-line function of the game (0x8000AB90). */
float DegToRad(float degrees);

/* The float overloads of std::sin and std::cos, out of line in the game (as in BAMine.cpp). */
namespace std {
float sin(float x);
float cos(float x);
} // namespace std

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);

/* GadgetModelInfo helpers (see Game/BASwitch.cpp, Game/Door.cpp and Game/EggBalloon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
const char *fn_801626D8(GadgetModelInfo *model);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169E8C(void *part, void *data);
/* The matrix of a character info's position and rotation (see Game/BKTank.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* The model file's table (see Game/Door.cpp). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
UnkSoundEntry_80208888 *fn_80208888(UnkModelTable_80208888 *table, const char *name);

/* The 32-bit flag word (see Game/BaseDoor.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The collision world, a probe along segment returning the first object for which filter is true
 * (ignoring the collider ignore), the hit object's position, and releasing the hit (see
 * Game/ContainerBehavior.cpp). */
void *fn_80020D40(void);
UnkCollision_80021E24 *fn_80021E24(void *world, void *segment, int arg2, int arg3,
                                   bool (*filter)(UnkCollision_80021E24 *hit),
                                   CharaColliBase *ignore);
const Vec3f *fn_8001E9BC(void *object);
void fn_800265A4(UnkCollision_80021E24 *hit, int arg1);

/* The attack collision of a bullet, its move from start to end, and its removal (see
 * Game/BulletCannon.cpp). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
HitResult fn_8005AFE4(int kind, const Vec3f *start, const Vec3f *end,
                      boost::shared_ptr<CharaColliAttack> *attack);
void fn_80053B40(boost::shared_ptr<CharaColliAttack> *attack);

/* Starts effect id at position, pointing along direction, and moves an effect (see
 * Game/ArkLaser.cpp and Game/BulletCannon.cpp). */
boost::weak_ptr<EffectElement> fn_80168C48(const Vec3f *position, const Vec3f *direction, int id);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);

/* Plays sound id at a position as a loop for source (see Game/BaseDoor.cpp). */
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);

/* Vector helpers (behaviour inferred): the matrix translated by (x, y, z) and scaled by (x, y, z),
 * the vector normalised, a direction turned into the angles of a rotation (see
 * Game/BulletCannon.cpp). */
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
void fn_8040B720(Mtx34f *matrix, float x, float y, float z);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_80016284(const Vec3f *direction);
}

/* fn_8040CD74 and fn_8040CDD0 called as returning a - b and a + b by value (they store the result
 * through their first argument; see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorOpFn)(const Vec3f *a, const Vec3f *b);
extern "C" void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b);

/* The default length of the beam, how much shorter than its reach it is drawn, and a third value
 * nothing reads (names inferred; the game sets them in __sinit; the inline call reproduces that, as
 * in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_defaultLength = DynamicFloat(100.0f);
static const float s_beamTrim = DynamicFloat(10.0f);
static const float s_unused = DynamicFloat(3.0f);

namespace SecurityLaser {

/* The parameters of the owner record (names from the parameter table). */
struct SetSecurityLaserParams {
    SetSecurityLaserParams()
        : model(0), length(s_defaultLength), rotTime(0.0f), rotY(0.0f), moveSec(0.0f), x(0.0f),
          y(0.0f), z(0.0f), type(0) {}

    /* 0x00 */ s32 model;
    /* 0x04 */ float length;
    /* 0x08 */ float rotTime;
    /* 0x0C */ float rotY; /* degrees */
    /* 0x10 */ float moveSec;
    /* 0x14 */ float x;
    /* 0x18 */ float y;
    /* 0x1C */ float z;
    /* 0x20 */ s32 type; /* 0 "Normal", 1 "Fixed", 2 "Fixed w/Eff" */
};

/* The flag word at 0xC0 (struct name unknown; bit 0: the beam stops at what it meets, bit 1: the
 * effect shows; meanings inferred). */
struct SetSecurityLaserFlags {
    SetSecurityLaserFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

class SetSecurityLaser : public Gadget {
  public:
    SetSecurityLaser(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetSecurityLaser();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and parameters and resets the state. */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the models and plays the loop sound. */
    void RebuildTransform();
    /* Sweeps the beam and swings the emitter. */
    void UpdateMotion(float dt);
    /* Finds where the beam ends and moves its collision and effect. */
    void UpdateBeam();
    /* Looks up the model's loop sound ("<model name>_lp"). */
    void FindSound();
    /* The callbacks of the attack collision (both empty). */
    void OnHit(AttackCallbackParam &param);
    void OnHitEnd(AttackCallbackParam &param);

    static const char *s_fileName;
    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_beamModel; /* variant 2, stretched */
    /* 0x48 */ boost::shared_ptr<GadgetModelInfo> m_tipModel;  /* variant 3, at the beam's end */
    /* 0x50 */ UnkModelTable_80208888 *m_table;
    /* 0x54 */ SetSecurityLaserParams m_params;
    /* 0x78 */ s32 m_modelIndex;
    /* 0x7C */ float m_moveTimer;
    /* 0x80 */ float m_moveSpeed; /* degrees a second */
    /* 0x84 */ Vec3f m_position;
    /* 0x90 */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x98 */ RwFrame *m_frame; /* where the beam leaves the model */
    /* 0x9C */ float m_rotTimer;
    /* 0xA0 */ float m_rotSpeed; /* degrees a second */
    /* 0xA4 */ float m_rotAngle;
    /* 0xA8 */ float m_beamLength;
    /* 0xAC */ u16 m_soundId;
    /* 0xB0 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0xB8 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0xC0 */ SetSecurityLaserFlags m_flags;
    /* 0xC4 */ u32 field_0xC4;
};
SIZE_ASSERT(SetSecurityLaser, 0xC8);

const char *SetSecurityLaser::s_fileName = "SECURITYLASER.BIN";
const char *SetSecurityLaser::s_taskName = "SetSecurityLaser";

/* Degrees to radians, inline (name inferred). */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The probe's filter: anything but the laser's own collider (name inferred). */
static bool IsNotSelf(UnkCollision_80021E24 *hit) {
    if (hit->field_0x8 != 0 && hit->field_0x8 == hit->field_0x10) {
        return false;
    }
    return true;
}

SetSecurityLaser::SetSecurityLaser(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x78, 4);
    LoadPlacement(owner);
    m_modelIndex = m_params.model;
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(m_modelIndex, 0);
    m_table = fn_80012700(fn_800122D4(), 1, s_fileName);
    m_beamModel = fn_8015F800(1, s_fileName);
    m_beamModel->fn_80162A68(2, 0);
    m_tipModel = fn_8015F800(1, s_fileName);
    m_tipModel->fn_80162A68(3, 0);
    fn_80161FC8(m_model.px, m_modelIndex, boost::weak_ptr<CharInfo>(m_chara));
    m_collider = fn_80161614(m_model.px, m_modelIndex);
    m_attack = fn_8005A300(1, boost::weak_ptr<CharInfo>(m_chara), 1.0f,
                           boost::bind(&SetSecurityLaser::OnHit, this, _1),
                           boost::bind(&SetSecurityLaser::OnHitEnd, this, _1));
    if (m_flags.Test(1)) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        Vec3f direction(0.0f, 0.0f, -1.0f);
        m_effect = fn_80168C48(&position, &direction, 0x4D);
    }
    FindSound();
    RebuildTransform();
}

SetSecurityLaser::~SetSecurityLaser() {
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect.get() != 0) {
            effect->Retire();
        }
        m_effect.reset();
    }
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
    }
}

void SetSecurityLaser::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_beamModel.px != 0) {
        m_beamModel->fn_80162FB0(dt);
    }
    if (m_tipModel.px != 0) {
        m_tipModel->fn_80162FB0(dt);
    }
    UpdateMotion(dt);
    RebuildTransform();
}

void SetSecurityLaser::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
    if (m_model.px != 0) {
        if (m_params.model != m_modelIndex) {
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_modelIndex = m_params.model;
            m_model->fn_80162A68(m_modelIndex, 0);
            fn_80161FC8(m_model.px, m_modelIndex, boost::weak_ptr<CharInfo>(m_chara));
            m_collider = fn_80161614(m_model.px, m_modelIndex);
        }
        Vec3f tip(0.0f, 0.0f, m_params.length);
        Mtx34f matrix;
        fn_8015CEA0(m_chara.px, &matrix);
        AssignVector(tip, fn_8040B67C(&matrix, &tip));
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &m_chara->m_position, &tip), &lbl_805F06D8);
        Vec3f offset(m_params.x, m_params.y, m_params.z);
        Vec3f rotation;
        ConstructVector(&rotation, fn_80016284(&offset));
        Vec3f start;
        ConstructVector(&start, ((VectorOpFn)fn_8040CDD0)(&m_chara.px->m_position, &offset));
        Vec3f end;
        ConstructVector(&end, ((VectorOpFn)fn_8040CDD0)(&m_chara.px->m_position, &rotation));
        u8 swing[0x18];
        fn_802E0B58(fn_80020A20(swing, &start, &end), &lbl_805F06D8);
        m_model->fn_8016108C();
    }
}

void SetSecurityLaser::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_moveTimer = 0.0f;
    float speed = 0.0f;
    if (speed != m_params.moveSec) {
        speed = 360.0f / m_params.moveSec;
    }
    m_moveSpeed = speed;
    fn_800091D0(&m_position, &m_chara->m_position);
    m_rotTimer = 0.0f;
    speed = 0.0f;
    if (speed != m_params.rotTime) {
        speed = 360.0f / m_params.rotTime;
    }
    m_rotSpeed = speed;
    m_rotAngle = 0.0f;
    m_frame = 0;
    m_beamLength = 0.0f;
    m_flags.Clear();
    switch (m_params.type) {
    case 0:
        m_flags.Set(0);
        m_flags.Set(1);
        break;
    case 1:
        break;
    case 2:
        m_flags.Set(1);
        break;
    }
    m_soundId = -1;
}

void SetSecurityLaser::RebuildTransform() {
    if (m_model.px != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.px, &matrix);
        m_model->fn_801633DC(&matrix);
    }
    UpdateBeam();
    if (m_frame == 0) {
        UnkFrameSearch_80169E8C search;
        search.id = 1;
        fn_8015FA78(m_model.px, fn_80169E8C, &search);
        m_frame = search.frame;
    } else if (m_beamModel.px != 0 && m_tipModel.px != 0) {
        RwMatrix *ltm = RwFrameGetLTM(m_frame);
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B52C(&matrix, ltm->pos.x, ltm->pos.y, ltm->pos.z);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        fn_8040B848(&matrix, m_rotAngle);
        fn_8040B720(&matrix, 1.0f, 1.0f, 0.1f * m_beamLength);
        m_beamModel->fn_801633DC(&matrix);
        fn_8040B6F8(&matrix);
        fn_8040B52C(&matrix, ltm->pos.x, ltm->pos.y, ltm->pos.z);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        fn_8040B848(&matrix, m_rotAngle);
        fn_8040B52C(&matrix, 0.0f, 0.0f, m_beamLength);
        m_tipModel->fn_801633DC(&matrix);
        if (m_flags.Test(1) && m_soundId != -1) {
            Vec3f position(0.0f, 0.0f, 0.0f);
            AssignVector(position, fn_8040B67C(&matrix, &position));
            fn_801D53C4(fn_80053164(), m_soundId, &position, this, 3, 0, 2);
        }
    }
}

void SetSecurityLaser::UpdateMotion(float dt) {
    if (m_params.moveSec > 0.0f) {
        m_moveTimer += dt;
        if (m_moveTimer > m_params.moveSec) {
            m_moveTimer = 0.0f;
        }
        float angle = m_moveSpeed * m_moveTimer;
        Vec3f offset(m_params.x, m_params.y, m_params.z);
        float length = fn_8040CF50(&offset);
        AssignVector(offset, fn_8040CEE8(&offset));
        fn_8040CE38(&offset, length * std::sin(DegToRad(angle)));
        AssignVector(m_chara.px->m_position, ((VectorOpFn)fn_8040CDD0)(&m_position, &offset));
    }
    if (m_params.rotTime > 0.0f) {
        m_rotTimer += dt;
        if (m_rotTimer > m_params.rotTime) {
            m_rotTimer = 0.0f;
        }
        float sweep = ToRadians(0.5f * m_params.rotY);
        m_rotAngle = sweep * std::cos(ToRadians(m_rotSpeed * m_rotTimer));
    }
}

void SetSecurityLaser::UpdateBeam() {
    if (m_frame == 0) {
        return;
    }
    bool stopped = false;
    m_beamLength = m_params.length;
    RwMatrix *ltm = RwFrameGetLTM(m_frame);
    Vec3f start(0.0f, 0.0f, 0.0f);
    Vec3f end(0.0f, 0.0f, m_beamLength);
    Mtx34f matrix;
    fn_8040B6F8(&matrix);
    fn_8040B52C(&matrix, ltm->pos.x, ltm->pos.y, ltm->pos.z);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_8040B848(&matrix, m_rotAngle);
    AssignVector(start, fn_8040B67C(&matrix, &start));
    AssignVector(end, fn_8040B67C(&matrix, &end));
    if (m_flags.Test(0)) {
        u8 segment[0x18];
        fn_80020A20(segment, &start, &end);
        boost::shared_ptr<CharaColliBase> collider = m_collider.lock();
        if (collider.get() != 0) {
            UnkCollision_80021E24 *hit =
                fn_80021E24(fn_80020D40(), segment, 3, 8, IsNotSelf, collider.get());
            if (hit != 0) {
                Vec3f position;
                fn_800091EC(&position, fn_8001E9BC(hit->m_object));
                m_beamLength = fn_8040CF50(&(const Vec3f &)((VectorOpFn)fn_8040CD74)(&start, &position));
                AssignVector(end, position);
                stopped = true;
                fn_800265A4(hit, 1);
            }
        }
    }
    m_beamLength -= s_beamTrim;
    if (m_beamLength < 0.0f) {
        m_beamLength = 0.0f;
    }
    fn_8005AFE4(7, &start, &end, &m_attack);
    if (!m_flags.Test(0) && m_flags.Test(1)) {
        stopped = true;
    }
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect.get() != 0) {
            fn_80425E4C(effect.get(), &end);
            if (stopped) {
                effect->Show();
            } else {
                effect->Hide();
            }
        }
    }
}

void SetSecurityLaser::FindSound() {
    if (m_model.px != 0) {
        const char *modelName = fn_801626D8(m_model.px);
        m_soundId = -1;
        if (modelName != 0) {
            char name[0x40];
            sprintf(name, "%s_lp", modelName);
            UnkSoundEntry_80208888 *entry = fn_80208888(m_table, name);
            if (entry != 0) {
                m_soundId = entry->soundId;
            }
        }
    }
}

void SetSecurityLaser::OnHit(AttackCallbackParam &param) {}

void SetSecurityLaser::OnHitEnd(AttackCallbackParam &param) {}

} // namespace SecurityLaser

/* The label of the "type" parameter (name inferred). */
extern "C" const char *fn_8022C4C4(UnkLabelQuery_800CBC20 *query) {
    static const char *s_types[3] = {"Normal", "Fixed", "Fixed w/Eff"};

    if (query->index == 8) {
        return s_types[query->value];
    }
    return 0;
}

extern "C" void fn_8022C498(void) {
    fn_8006B474("SECURITYLASER.BIN", 1);
}

extern "C" void fn_8022C494(void) {}

namespace SecurityLaser {

void SetSecurityLaser::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetSecurityLaser(parent, owner);
}

} // namespace SecurityLaser

static GadgetParamDesc lbl_80546C5C[] = {
    {1, "model", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "length", 0, 0, 0, s_defaultLength, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "rot time(sec)", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "rot Y(deg)", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "move sec", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {1, "type", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8022C4C4, 0},
    {0},
};

GadgetClassDesc lbl_80546DEC = {
    "SECURITY LASER", fn_8022C498, fn_8022C494, SecurityLaser::SetSecurityLaser::Instantiate, 0, 0,
    0x13EF, 1, 0, 10, lbl_80546C5C,
};
