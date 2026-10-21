/* ElecBlock (class name from RTTI): the ELEC_BIGBLOCK gadget of the electric circuit area. It shows
 * one of the blocks of BIGBLOCK.BIN, picked by its "BlockType" parameter and scaled by its "Scale"
 * parameters, raised or lowered by its "Initial Y" parameter. Once a character comes within its
 * search range it moves back to its resting height, accelerating up or down, and settles there.
 * File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <cmath>

#include "Game/CharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class UnkSoundManager_80053164;

extern "C" {
/* Attaches a character info to model number index (behaviour inferred). */
bool fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
/* Passes offset to each part of the model whose id equals arg1 (behaviour inferred; meanings of
 * the integer arguments unknown). */
void fn_80161740(GadgetModelInfo *model, int arg1, const Vec3f *offset, int arg3);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Returns a manager, constructing it on first use (behaviour inferred); fn_801D54F4 plays sound
 * id at the character info's position through it (behaviour inferred; the meanings of the last
 * two arguments are unknown). */
UnkSoundManager_80053164 *fn_80053164(void);
void fn_801D54F4(UnkSoundManager_80053164 *manager, int id, GadgetCharInfo *charInfo, int arg3,
                 int arg4);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct ElecBlockParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The seven parameters the owner supplies (names from the parameter table). */
struct ElecBlockParams {
    /* 0x00 */ int blockType;
    /* 0x04 */ float scaleX;
    /* 0x08 */ float scaleY;
    /* 0x0C */ float scaleZ;
    /* 0x10 */ float searchRange;
    /* 0x14 */ float initialY;
    /* 0x18 */ int collisionFlag;
};
SIZE_ASSERT(ElecBlockParams, 0x1C);

class ElecBlock : public Gadget {
  public:
    ElecBlock(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecBlock();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the block under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Places the model at the character info's translation, raised by m_height, turned about the
     * Y axis and scaled (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Advances *position and *velocity by dt under acceleration, slowed by drag in proportion to
     * the velocity when drag is positive: the exact solution of v' = acceleration - drag * v, with
     * e^(drag * dt) from pow (name inferred). */
    void Integrate(float *position, float *velocity, float acceleration, float drag, float dt);
    /* True when the nearest character info is within the search range (name inferred). */
    bool IsWithinReach();
    /* Passes the scale to the model and lowers it by its scaled height (name inferred). */
    void ResizeToScale();

    static const char *s_taskName;

    /* 0x30 */ ElecBlockParams m_params;
    /* 0x4C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x54 */ GadgetModelInfo *m_model;
    /* 0x58 */ int m_blockType; /* the block shown (name inferred) */
    /* 0x5C */ int m_state;     /* 0 waiting, 1 moving, 2 settling, 3 at rest (inferred; nothing
                                 * in this unit sets 2) */
    /* 0x60 */ float m_height;  /* offset from the resting height (inferred) */
    /* 0x64 */ float m_speed;   /* names inferred */
    /* 0x68 */ boost::shared_ptr<CharInfo> field_0x68;
};
SIZE_ASSERT(ElecBlock, 0x70);

const char *ElecBlock::s_taskName = "ElecBlock";

/* The unscaled size of a block (inferred from ResizeToScale). */
static Vec3f lbl_80580150(86.5f, 195.0f, 86.5f);

/* Advances *position by dt at acceleration, the velocity capped at maxSpeed (name inferred). The
 * original repeats this code for both directions in Behave; whether it was written out twice or
 * inlined from a helper is unknown. */
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

ElecBlock::ElecBlock(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(0x56, 4, "ElecBlock");
    m_model = new GadgetModelInfo(1, "BIGBLOCK.BIN");
    m_blockType = m_params.blockType;
    m_model->fn_80162A68(m_blockType, 0);
    ReadPlacement(m_owner);
    if (m_params.collisionFlag != 0) {
        m_model->fn_80161F08("col", m_chara);
        ResizeToScale();
    }
    m_height = m_params.initialY;
    if (0.0f == m_params.initialY) {
        m_state = 3;
    } else {
        m_state = 0;
    }
    m_speed = 0.0f;
    RebuildTransform();
}

ElecBlock::~ElecBlock() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecBlock::Behave(float dt) {
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
        if (m_params.initialY < 0.0f) {
            MoveCapped(&m_height, &m_speed, 200.0f, 2000.0f, dt);
            if (m_height > 0.0f) {
                m_height = 0.0f;
                m_state = 3;
            }
        } else {
            MoveCapped(&m_height, &m_speed, -200.0f, 2000.0f, dt);
            if (m_height < 0.0f) {
                m_height = 0.0f;
                m_state = 3;
            }
        }
        break;
    case 2:
        Integrate(&m_height, &m_speed, -1.0f * m_height, 10.0f, dt);
        if (std::fabs(m_speed) < 0.001667f) {
            m_height = 0.0f;
            m_state = 3;
        }
        break;
    case 3:
        /* At rest. Listing the case keeps the original's comparison order (2 first). */
        break;
    }

    RebuildTransform();
}

void ElecBlock::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    m_state = 0;
    m_height = m_params.initialY;
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        if (m_params.blockType != m_blockType) {
            m_model->fn_80161880();
            m_model->fn_801604FC();
            m_model->fn_80162804();
            m_blockType = m_params.blockType;
            m_model->fn_80162A68(m_blockType, 0);
            fn_80161FC8(m_model, m_blockType, m_chara);
            m_model->fn_80160C28(m_blockType);
            m_params.blockType = m_blockType;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
        if (m_params.collisionFlag != 0) {
            ResizeToScale();
            m_model->fn_8016108C();
        }
        fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.searchRange);
    }
}

void ElecBlock::ResizeToScale() {
    float height = lbl_80580150.y * m_params.scaleY;

    m_model->fn_801617D0(0, -2, lbl_80580150.x * m_params.scaleX, height,
                         lbl_80580150.z * m_params.scaleZ);
    Vec3f offset(0.0f, -height, 0.0f);
    fn_80161740(m_model, 0, &offset, -2);
}

bool ElecBlock::IsWithinReach() {
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

void ElecBlock::Integrate(float *position, float *velocity, float acceleration, float drag,
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

void ElecBlock::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void ElecBlock::RebuildTransform() {
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

extern "C" void fn_80158684(void) {
    fn_8006B474("BIGBLOCK.BIN", 1);
}

extern "C" void fn_80158680(void) {}

void ElecBlock::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecBlock(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The labels of the BlockType and CollisionFlag values. */
static const char *lbl_8052AB58[] = {
    "BlockType0", "BlockType1", "BlockType2", "BlockType3", "BlockType4",
    "BlockType5", "BlockType6", "BlockType7", "BlockType8", "BlockType9",
};

static const char *lbl_805E6524[] = {
    "CollisionFlag< FALSE >",
    "CollisionFlag< TRUE  >",
};

/* The label callback of the BlockType and CollisionFlag parameters. */
extern "C" const char *fn_801585DC(UnkLabelQuery_800CBC20 *cursor) {
    const ElecBlockParamCursor *fields = (const ElecBlockParamCursor *)cursor;

    switch (fields->index) {
    case 0:
        return lbl_8052AB58[fields->value];
    case 6:
        return lbl_805E6524[fields->value];
    }
    return 0;
}

static GadgetParamDesc lbl_8052AB80[] = {
    {1, "BlockType", 0, 0, 9, 0.0f, 0.0f, 0.0f, fn_801585DC, 0},
    {4, "Scale X", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Scale Y", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Scale Z", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Initial Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {1, "CollisionFlag", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_801585DC, 0},
    {0},
};

GadgetClassDesc lbl_8052ACC0 = {
    "ELEC_BIGBLOCK", fn_80158684, fn_80158680, ElecBlock::Instantiate, 0, 0, 0x7D5, 1, 0, 10,
    lbl_8052AB80,
};
