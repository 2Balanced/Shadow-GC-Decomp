/* ElecFireWall (class name from RTTI): the ELEC_FIREWALL gadget of the electric circuit area. It
 * shows FIREWALL.BIN scaled by its "Scale" parameters and lowered by its "Initial Y" parameter.
 * Once a character comes within its search range it rises, accelerating, overshoots and settles at
 * its resting height, scaling a frame of the model as it goes. Its attack collider's callback marks
 * it hit; it then breaks into pieces, plays a sound and reports itself finished. File name inferred
 * from the class. Method names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * The constructor addresses its two member pointer constants and the vtable through the start of
 * .data, so the linker keeps this unit's .data whole: the RTTI base lists the two first headers
 * produce lead it (in this order), and the copies of the weak exception and sp_counted_base vtables
 * close it, zeroed because the copies of other units are the ones linked. */
#include <cmath>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/TNumEdit.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* An attack collision (class name from the RTTI of its shared pointer's deleter; see
 * Game/CharaColliAttack.h); its layout is not needed here. */
class CharaColliAttack;

/* What an attack collision passes to its callbacks (struct name from the bound callbacks' RTTI).
 * Game/CharaColliAttack.h maps the attacker and the answer; this unit also reads the direction at
 * 0x34 and its length at 0x4C (names inferred), so the struct is declared here. */
struct AttackCallbackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x34 - 0x8];
    /* 0x34 */ Vec3f m_direction; /* normalised here */
    /* 0x40 */ u8 field_0x40[0x4C - 0x40];
    /* 0x4C */ float m_length;
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

class UnkSoundManager_80053164;
class UnkPieceManager_800122D4;

/* The RenderWare types used here, as in rw/rwcore.h (which needs the C library headers). */
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

/* What fn_8015FA78 passes for each part of a model (see ElecBomb.cpp). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ RwObject *m_object;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

/* A model part as fn_8015F7BC returns it; only the word at 0x4 is used (layout unknown). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ void *field_0x4;
};

extern "C" {
/* RenderWare's RwFrameScale and RwFrameTransform (rw/rwcore.h); the combine operations are
 * replace (0) and post-concatenate (2). */
RwFrame *fn_8048764C(RwFrame *frame, const RwV3d *scale, int combine);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *transform, int combine);
/* Calls callback for each part of model while it returns non-null (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback, void *data);
/* Returns the part of model whose id is id (behaviour inferred). */
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* Passes offset to each part of the model whose id equals arg1 (see ElecBlock.cpp). */
void fn_80161740(GadgetModelInfo *model, int arg1, const Vec3f *offset, int arg3);
/* Returns the attack collider of model number index (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
/* Sets the attack's two callbacks (behaviour and the other arguments' meanings inferred). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2,
                 const boost::function1<void, AttackCallbackParam &> &onAttack,
                 const boost::function1<void, AttackCallbackParam &> &onEnd, float arg5);
/* The pieces' manager, the setup and launch of the pieces, and a lookup the pieces need (behaviour
 * inferred). */
UnkPieceManager_800122D4 *fn_800122D4(void);
u32 fn_80012700(UnkPieceManager_800122D4 *manager, int arg1, void *arg2);
/* Normalises vector and returns its former length (see fn_8040CE74.cpp). */
float fn_8040CE74(Vec3f *vector);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Returns the sound manager and plays sound id through it, at the character info's position for
 * fn_801D54F4 (behaviour inferred). */
UnkSoundManager_80053164 *fn_80053164(void);
int fn_801D560C(UnkSoundManager_80053164 *manager, int id, int arg2, int arg3);
void fn_801D54F4(UnkSoundManager_80053164 *manager, int id, GadgetCharInfo *charInfo, int arg3,
                 int arg4);
}

/* The five parameters the owner supplies (names from the parameter table). */
struct ElecFireWallParams {
    /* 0x00 */ float scaleX;
    /* 0x04 */ float scaleY;
    /* 0x08 */ float scaleZ;
    /* 0x0C */ float searchRange;
    /* 0x10 */ float initialY;
};
SIZE_ASSERT(ElecFireWallParams, 0x14);

/* What the frame search callback looks for and finds (names inferred). */
struct ElecFireWallFrameSearch {
    ElecFireWallFrameSearch() : index(0), frame(0) {}

    /* 0x0 */ s32 index;
    /* 0x4 */ RwFrame *frame;
};

class ElecFireWall : public Gadget {
  public:
    ElecFireWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecFireWall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the wall under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Places the model at the character info's translation, raised by m_height, turned about the
     * Y axis and scaled (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Advances *position and *velocity by dt under acceleration with drag (see ElecBlock.cpp;
     * name inferred). */
    void Integrate(float *position, float *velocity, float acceleration, float drag, float dt);
    /* True when the nearest character info is within the search range (name inferred). */
    bool IsWithinReach();
    /* Passes the scale to the model and raises it by its scaled height (name inferred). */
    void ResizeToScale();
    /* Scales the model's frame 1 vertically by m_frameScale (name inferred). */
    void UpdateFrameScale();
    /* The attack collider's callbacks: the first marks the wall hit (names inferred). */
    void OnAttack(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    /* Breaks the model into pieces and plays the breaking sound (name inferred). */
    void Break();

    static const char *s_taskName;

    /* 0x30 */ ElecFireWallParams m_params;
    /* 0x44 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x4C */ GadgetModelInfo *m_model;
    /* 0x50 */ int m_state;       /* 0 waiting, 1 rising, 2 settling, 3 at rest, 4 hit (inferred) */
    /* 0x54 */ float m_height;    /* names inferred */
    /* 0x58 */ float m_speed;
    /* 0x5C */ float m_frameScale;
    /* 0x60 */ RwFrame *m_frame;
    /* 0x64 */ RwMatrix m_frameMatrix;
    /* 0xA4 */ boost::shared_ptr<CharInfo> field_0xA4; /* not used by this unit */
    /* 0xAC */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0xB4 */ u32 field_0xB4;
};
SIZE_ASSERT(ElecFireWall, 0xB8);

const char *ElecFireWall::s_taskName = "ElecFireWall";

/* The unscaled size of the wall (inferred from ResizeToScale). */
static Vec3f lbl_80583B68(86.5f, 195.0f, 86.5f);

/* Advances *position by dt at acceleration, the velocity capped at maxSpeed (see ElecBlock.cpp;
 * name inferred). */
inline void MoveCapped(float *position, float *velocity, float acceleration, float maxSpeed,
                       float dt) {
    float v0 = *velocity;
    float accelTime = dt;
    float topTime = 0.0f;

    if (v0 >= maxSpeed) {
        accelTime = 0.0f;
        topTime = dt;
    } else {
        float dv = acceleration * dt;
        float v1 = v0 + dv;
        if (v1 > maxSpeed) {
            *velocity = maxSpeed;
            accelTime = (maxSpeed - v0) / acceleration;
            topTime = dt - accelTime;
        } else {
            *velocity = v1;
        }
    }
    *position = *position + v0 * accelTime + acceleration * accelTime * accelTime * 0.5f +
                maxSpeed * topTime;
}

/* Defined before the constructor: the original emits it after the constructor's template
 * instances. */

/* Part callback: stores the frame of the part whose index is the one searched for, and stops the
 * walk once it has one (name inferred). */
static UnkModelPart_8015FA78 *FindFrame(UnkModelPart_8015FA78 *part, void *data) {
    ElecFireWallFrameSearch *search = static_cast<ElecFireWallFrameSearch *>(data);

    if (search != 0 && part->m_object != 0 && part->m_index == search->index) {
        search->frame = static_cast<RwFrame *>(part->m_object->parent);
        if (search->frame != 0) {
            return 0;
        }
    }
    return part;
}

ElecFireWall::ElecFireWall(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(0x56, 4, "ElecFireWall");
    m_model = new GadgetModelInfo(1, "FIREWALL.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    m_attack = fn_80161428(m_model, 0);
    if (boost::shared_ptr<CharaColliAttack> attack = m_attack.lock()) {
        fn_80054DD0(attack.get(), 1, 1, boost::bind(&ElecFireWall::OnAttack, this, _1),
                    boost::bind(&ElecFireWall::OnAttackEnd, this, _1), 1.0f);
    }
    ReadPlacement(m_owner);
    m_height = m_params.initialY;
    if (m_height > -100.0f * m_params.scaleY) {
        m_height = -100.0f * m_params.scaleY;
    }
    ResizeToScale();
    if (0.0f == m_params.initialY) {
        m_state = 3;
    } else {
        m_state = 0;
    }
    m_speed = 0.0f;
    m_frameScale = 1.0f;
    RebuildTransform();
}

ElecFireWall::~ElecFireWall() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecFireWall::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (IsWithinReach()) {
            m_state = 1;
            fn_801D54F4(fn_80053164(), 0x723D, m_chara.get(), 0, 1);
        }
        break;
    case 1:
        MoveCapped(&m_height, &m_speed, 200.0f, 2000.0f, dt);
        if (m_height > -100.0f * m_params.scaleY) {
            m_height = -97.0f * m_params.scaleY;
            m_speed *= 0.333f;
            m_state = 2;
        }
        break;
    case 2:
        Integrate(&m_height, &m_speed, -1.0f * (m_height - -100.0f * m_params.scaleY), 25.0f, dt);
        if (std::fabs(m_speed) < 0.001667f) {
            m_height = -100.0f * m_params.scaleY;
            m_state = 3;
        }
        break;
    case 4:
        Break();
        fn_8015A794(this);
        m_flags |= 1;
        return;
    }

    RebuildTransform();
    ResizeToScale();
    if (m_state != 4) {
        float low = -370.0f * m_params.scaleY;
        float height = m_height;
        if (height < low) {
            m_frameScale = 1.0f;
        } else {
            m_frameScale = 1.0f - (low - height) / low;
        }
        UpdateFrameScale();
    }
}

void ElecFireWall::UpdateFrameScale() {
    if (m_model != 0) {
        ElecFireWallFrameSearch search;

        search.index = 1;
        search.frame = 0;
        fn_8015FA78(m_model, FindFrame, &search);
        m_frame = search.frame;
        m_frameMatrix = search.frame->modelling;
        if (m_frame != 0) {
            RwV3d scale = {1.0f, 0.0f, 1.0f};

            scale.y = m_frameScale;
            fn_8048764C(m_frame, &scale, 0);
            fn_804876E8(m_frame, &m_frameMatrix, 2);
        }
    }
}

void ElecFireWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    m_state = 0;
    m_height = m_params.initialY;
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        ResizeToScale();
        m_model->fn_8016108C();
        fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.searchRange);
    }
}

void ElecFireWall::ResizeToScale() {
    float height = lbl_80583B68.y * m_params.scaleY;

    m_model->fn_801617D0(0, -2, lbl_80583B68.x * m_params.scaleX, height,
                         lbl_80583B68.z * m_params.scaleZ);
    Vec3f offset(0.0f, height + m_height, 0.0f);
    fn_80161740(m_model, 0, &offset, -2);
}

bool ElecFireWall::IsWithinReach() {
    boost::shared_ptr<CharInfo> nearest;

    nearest = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
    if (nearest.get() != 0) {
        float reachSq = m_params.searchRange * m_params.searchRange;
        Vec3f offset;

        fn_8040CD74(&offset, &m_chara->m_position, &nearest->m_position);
        return fn_8040CF2C(&offset) <= reachSq;
    }

    return false;
}

void ElecFireWall::Integrate(float *position, float *velocity, float acceleration, float drag,
                             float dt) {
    if (drag <= 0.0f) {
        float dv = acceleration * dt;
        *position = *position + *velocity * dt + dt * dv / 2.0f;
        *velocity = *velocity + dv;
    } else {
        float decay = std::pow(2.7182817f, drag * dt);
        float invDecay = 1.0f / decay;
        float invDrag = 1.0f / drag;
        *position = *position + dt * (acceleration * invDrag) +
                    (1.0f - invDecay) * (invDrag * (invDrag * (drag * *velocity - acceleration)));
        *velocity = invDecay * (*velocity - acceleration * invDrag) + acceleration * invDrag;
    }
}

void ElecFireWall::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void ElecFireWall::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        Vec3f offset(0.0f, m_height, 0.0f);
        fn_8040B59C(&matrix, &offset);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B720(&matrix, m_params.scaleX, m_params.scaleY, m_params.scaleZ);
        m_model->fn_801633DC(&matrix);
    }
}

void ElecFireWall::OnAttack(AttackCallbackParam &param) {
    fn_800091D0(&param.m_direction, &m_chara->field_0x18);
    param.m_length = fn_8040CF50(&param.m_direction);
    fn_8040CE74(&param.m_direction);
    if (0.0f == param.m_length) {
        Vec3f up(0.0f, 1.0f, 0.0f);
        fn_800091D0(&param.m_direction, &up);
        param.m_length = 1.0f;
    }
    m_state = 4;
}

void ElecFireWall::OnAttackEnd(AttackCallbackParam &param) {}

void ElecFireWall::Break() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 3);
        if (part != 0) {
            UnkPieceManager_800122D4 *manager = fn_800122D4();
            UnkDebrisParam_8015F0B8 pieces(m_chara, 0);

            pieces.scale = 1.3f;
            pieces.model = fn_80012700(manager, 1, part->field_0x4);
            pieces.field_0x54 = 0;
            pieces.field_0x4C = 150;
            fn_8015D93C(&pieces);
        }
    }
    fn_801D560C(fn_80053164(), 0x723E, 0, 0);
}

extern "C" void fn_801F6634(void) {
    fn_8006B474("FIREWALL.BIN", 1);
}

extern "C" void fn_801F6630(void) {}

void ElecFireWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecFireWall(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80542128[] = {
    {4, "Scale X", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Scale Y", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Scale Z", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Initial Y", 0, 0, 0, -370.0f, -65536.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80542218 = {
    "ELEC_FIREWALL", fn_801F6634, fn_801F6630, ElecFireWall::Instantiate, 0, 0, 0x7DF, 1, 0, 10,
    lbl_80542128,
};
