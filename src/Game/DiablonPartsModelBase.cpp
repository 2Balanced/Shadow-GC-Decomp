/* Boss::Diablon::Parts::ModelBase (class name from RTTI; declared in Game/Diablon.h): a part of the
 * Diablon boss with a model of its own. The model (UnkModel_802DB4D0, class name unknown) loads a
 * clump from the boss's resource slot into the stage's world, plays the part's motions on it, keeps
 * it on the part's parent matrix and keeps the matrices of some of its frames for the parts below.
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE

#include "Game/CharInfo.h"
#include "Game/DebugDispShape.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct _stMtnPlayWork;

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/GadgetModelInfo.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* The stage (class name unknown; fn_8003A840 returns it): the world of each of its areas (as in
 * Game/BossCtrlUnit.cpp). */
class UnkStage_8003A840 {
  public:
    void *GetWorld(int area);
};

/* What CharInfo's word at 0x60 points to (struct name unknown; see Game/DiablonPartsBody.cpp). */
struct UnkCharTime {
    /* 0x0 */ float m_frameTime;
    /* 0x4 */ float m_speed; /* (name inferred) */
};

extern "C" {
/* The resource manager, a model of a slot, the number of motions of a slot and its motion data
 * (see Game/GadgetModelInfo.cpp; behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *manager, s32 slot, int model);
int fn_80012608(void *manager, s32 slot, int type);
void *fn_80012684(void *manager, s32 slot, int anim);
/* The stage, the area a clump's frame lies in (see Game/BossCtrlUnit.cpp). */
UnkStage_8003A840 *fn_8003A840(void);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
/* RpClumpClone, the clump's set-up and fade (behaviour inferred), RpClumpDestroy. */
RpClump *fn_80457090(RpClump *clump);
void fn_80038C84(RpClump *clump);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80457468(RpClump *clump);
/* The world a clump is in, and taking it out of and adding it to a world. */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The frame of clump with hierarchy id id, the root of a frame and the world matrix of a frame
 * (behaviour inferred). */
RwFrame *fn_80038D24(RpClump *clump, s32 id);
RwFrame *fn_804872D8(RwFrame *frame);
Mtx34f fn_802C8068(RwFrame *frame);
/* The motion player (see Game/GadgetModelInfo.cpp and Game/BlackDoomModeCtrlUnit.cpp). */
_stMtnPlayWork *fn_8041B908(void *anim, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_800163F4(Mtx34f *out, const Mtx34f *matrix);
void fn_804172D8(_stMtnPlayWork *work, const Mtx34f *matrix);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80415EA4(_stMtnPlayWork *work);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
int fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
void fn_80416014(_stMtnPlayWork *work, int arg1, float value);
void fn_80416198(_stMtnPlayWork *work, int arg1);
void fn_80415F98(_stMtnPlayWork *work, int arg1, float arg2, int motion, float time);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
/* A 32-bit flag word (see Game/BossSonic.h): construct it cleared, set, test and clear a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
}

/* Moves clump into world (as in Game/GadgetModelInfo.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Copies value into matrix (as in Game/DiablonPartsBeam.cpp). */
inline void AssignMatrix(Mtx34f &matrix, const Mtx34f &value) {
    fn_8040C148(&matrix, &value);
}

namespace Boss {
namespace Diablon {
namespace Parts {

/* Flag bits of the model (meanings inferred): bits 0-15 are the motion's events this frame. */
enum {
    kModelFlagBlending = 0x11, /* SetMotionNode set up a frame blend */
    kModelFlagQueued = 0x12,   /* PlayMotions queued the next motion */
};

class UnkModel_802DB4D0 {
  public:
    /* A frame of the model and its last world matrix (struct name inferred). */
    struct Node {
        Node() {}
        ~Node() {}

        /* 0x00 */ RwFrame *m_frame;
        /* 0x04 */ Mtx34f m_matrix;
    };

    UnkModel_802DB4D0(const boost::shared_ptr<Diablon> &owner, const ModelDesc *desc);
    ~UnkModel_802DB4D0();

    /* Names inferred. */
    const Mtx34f *GetNodeMatrix(int index);
    void Tick();
    void LoadClump(const ModelDesc *desc);
    void SetMotionNode(int node, int motion);
    void PlayMotions(int motion, int next, float blend);
    void UpdateFrame();
    void SetupMotion(const ModelDesc *desc);
    void UpdateNodes();
    void SetupNodes(const ModelDesc *desc);
    void StoreNodes();

    void PlayMotion(int motion, float blend) {
        if (motion < m_motionCount && m_motion != 0) {
            if (fn_80014E34(&m_flags, kModelFlagQueued)) {
                fn_8006127C(&m_flags, kModelFlagQueued);
                m_motionIndex = -1;
            }
            if (fn_80014E34(&m_flags, kModelFlagBlending)) {
                fn_80416198(m_motion, 0);
                fn_8006127C(&m_flags, kModelFlagBlending);
            }
            if (m_motionIndex != motion) {
                m_motionIndex = motion;
                if (blend > 1.0f) {
                    fn_804165D8(m_motion, 0, -1, -1);
                    fn_80416148(m_motion, 0, blend);
                }
                fn_80416014(m_motion, 0, 0.0f);
                if (m_motions[m_motionIndex].m_id < 0) {
                    fn_80417568(m_motion, 0xFFFF);
                } else {
                    fn_80417568(m_motion, m_motionBase + m_motions[m_motionIndex].m_id);
                }
                fn_80416F28(m_motion, 0, 0xB, 0);
                m_motionArg = m_motions[m_motionIndex].field_0x4;
            }
        }
    }

    /* Sets flag bits 0-15 to the motion's events this frame (name inferred). */
    void UpdateEvents() {
        int blending = fn_80014E34(&m_flags, kModelFlagBlending) ? 1 : 0;
        for (int i = 0; i < 16; i++) {
            if (fn_80416F28(m_motion, blending, i, 1) != 0) {
                fn_80014E1C(&m_flags, i);
            } else {
                fn_8006127C(&m_flags, i);
            }
        }
    }

    /* 0x00 */ boost::shared_ptr<Diablon> m_owner;
    /* 0x08 */ const Mtx34f *m_parentMatrix;
    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ _stMtnPlayWork *m_motion;
    /* 0x14 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x18 */ s32 m_motionIndex;
    /* 0x1C */ s32 m_motionArg;
    /* 0x20 */ s32 m_motionBase;
    /* 0x24 */ s32 m_motionCount;
    /* 0x28 */ const ModelNode *m_motions;
    /* 0x2C */ s32 m_nodeCount;
    /* 0x30 */ boost::scoped_array<Node> m_nodes;
    /* 0x34 */ u32 m_flags;
};
SIZE_ASSERT(UnkModel_802DB4D0, 0x38);

/* Deletes the model (name inferred; see checked_delete below). */
inline void DeleteModel(UnkModel_802DB4D0 *model) {
    delete model;
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

/* Retail keeps boost::checked_delete<UnkModel_802DB4D0> out of line (0x802DAB48) and the model's
 * destructor at its own place; this specialization calls an inline helper, which gives retail's
 * code and order (as in Game/DiablonExecuter.cpp; see docs/research-notes.md). */
namespace boost {
template <>
inline void checked_delete<Boss::Diablon::Parts::UnkModel_802DB4D0>(
    Boss::Diablon::Parts::UnkModel_802DB4D0 *x) {
    Boss::Diablon::Parts::DeleteModel(x);
}
} // namespace boost

namespace Boss {
namespace Diablon {
namespace Parts {

UnkModel_802DB4D0::UnkModel_802DB4D0(const boost::shared_ptr<Diablon> &owner,
                                     const ModelDesc *desc)
    : m_owner(owner) {
    fn_80014E04(&m_flags);
    m_parentMatrix = 0;
    m_clump = 0;
    m_motion = 0;
    m_updater = 0;
    m_motionIndex = -1;
    m_motionArg = 0;
    m_motionBase = 0;
    m_motionCount = 0;
    m_motions = 0;
    m_nodeCount = 0;
    LoadClump(desc);
    SetupMotion(desc);
    SetupNodes(desc);
    PlayMotion(0, 0.0f);
}

UnkModel_802DB4D0::~UnkModel_802DB4D0() {
    StoreNodes();
    if (m_updater != 0) {
        delete m_updater;
    }
    if (m_motion != 0) {
        fn_8041B884(m_motion);
    }
    m_updater = 0;
    m_motion = 0;
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

const Mtx34f *UnkModel_802DB4D0::GetNodeMatrix(int index) {
    return &m_nodes[index].m_matrix;
}

void UnkModel_802DB4D0::Tick() {
    UpdateFrame();
    UpdateNodes();
}

void UnkModel_802DB4D0::LoadClump(const ModelDesc *desc) {
    UnkStage_8003A840 *stage;
    void *manager = fn_800122D4();
    stage = fn_8003A840();
    m_clump = fn_8001277C(manager, m_owner->m_cameraSlot, desc->field_0x0);

    if (m_clump != 0) {
        m_clump = fn_80457090(m_clump);
        fn_80038C84(m_clump);
        int area = fn_80169350(fn_80169364(m_clump));
        RpClump *clump = m_clump;
        SetClumpWorld(clump, stage->GetWorld(area));
        fn_80038198(m_clump, &m_owner->m_fade);
    }
}

void UnkModel_802DB4D0::SetMotionNode(int node, int motion) {
    if (!fn_80014E34(&m_flags, kModelFlagQueued)) {
        if (fn_80014E34(&m_flags, kModelFlagBlending)) {
            fn_80416198(m_motion, 0);
        }
        fn_804165D8(m_motion, 2, -1, -1);
        fn_80417568(m_motion, m_motionBase + m_motions[node].m_id);
        fn_80416F28(m_motion, 0, 0xB, 0);
        fn_80416014(m_motion, 0, 0.0f);
        fn_80415F98(m_motion, 0, 0.0f, motion, 1.0f);
        fn_80416DC8(m_motion, 0, reinterpret_cast<UnkCharTime *&>(m_owner->field_0x60)->m_speed);
        fn_80014E1C(&m_flags, kModelFlagBlending);
    }
}

void UnkModel_802DB4D0::PlayMotions(int motion, int next, float blend) {
    PlayMotion(motion, blend);
    if (fn_804165D8(m_motion, 1, -1, -1) != 0) {
        fn_80417568(m_motion, m_motionBase + m_motions[next].m_id);
        fn_80416F28(m_motion, 0, 0xB, 0);
        fn_80014E1C(&m_flags, kModelFlagQueued);
    }
}

void UnkModel_802DB4D0::UpdateFrame() {
    if (m_clump != 0 && m_parentMatrix != 0) {
        RwFrame *frame = fn_804872D8(m_clump->frame);
        if (m_motion != 0) {
            Mtx34f matrix;
            fn_800163F4(&matrix, m_parentMatrix);
            fn_804172D8(m_motion, &matrix);
            fn_80416DC8(m_motion, 0,
                        reinterpret_cast<UnkCharTime *&>(m_owner->field_0x60)->m_speed);
            fn_80415EA4(m_motion);
            if (m_updater != 0) {
                fn_800159AC(m_updater);
            }
        } else {
            frame->modelling = fn_800591F0(m_parentMatrix);
            fn_8048722C(frame);
        }
    }
}

void UnkModel_802DB4D0::SetupMotion(const ModelDesc *desc) {
    void *manager = fn_800122D4();
    m_motionBase = fn_80012608(manager, m_owner->m_cameraSlot, 0xC) - 1;

    m_motionCount = desc->m_nodeCount;
    m_motions = desc->m_nodes;
    if (desc->field_0x4 >= 0) {
        m_motion = fn_8041B908(fn_80012684(manager, m_owner->m_cameraSlot, desc->field_0x4), 2);

        m_updater = new UnkClumpUpdater_80015A7C(m_clump, m_motion);
    }
}

void UnkModel_802DB4D0::UpdateNodes() {
    for (int i = 0; i < m_nodeCount; i++) {
        AssignMatrix(m_nodes[i].m_matrix, fn_802C8068(m_nodes[i].m_frame));
    }
}

void UnkModel_802DB4D0::SetupNodes(const ModelDesc *desc) {
    m_nodeCount = desc->m_motionCount;
    m_nodes.reset(new Node[m_nodeCount]);
    for (int i = 0; i < m_nodeCount; i++) {
        m_nodes[i].m_frame = fn_80038D24(m_clump, desc->m_motions[i]);
    }
}

void UnkModel_802DB4D0::StoreNodes() {
    for (int i = 0; i < m_nodeCount; i++) {
        AssignMatrix(m_nodes[i].m_matrix, fn_802C8068(m_nodes[i].m_frame));
    }
}

ModelBase::ModelBase(PartsBase *parent, const char *name, const boost::shared_ptr<Diablon> &owner,
                     const ModelDesc *desc, const Mtx34f *parentMatrix)
    : PartsBase(parent, name, owner), m_model(new UnkModel_802DB4D0(owner, desc)) {
    if (parentMatrix != 0) {
        SetParentMatrix(parentMatrix);
    }
}

ModelBase::~ModelBase() {}

void ModelBase::Tick(float dt) {
    m_model->Tick();
    m_model->UpdateEvents();
}

void ModelBase::PlayMotion(int motion, float blend) {
    m_model->PlayMotion(motion, blend);
}

void ModelBase::SetMotionNode(int node, int motion) {
    m_model->SetMotionNode(node, motion);
}

void ModelBase::PlayMotions(int motion, int next, float blend) {
    m_model->PlayMotions(motion, next, blend);
}

void ModelBase::SetMotionSpeed(float speed) {
    fn_80416014(m_model->m_motion, 0, speed);
}

int ModelBase::GetMotionIndex() {
    return m_model->m_motionIndex;
}

void *ModelBase::GetMotion() {
    return m_model->m_motion;
}

const Mtx34f *ModelBase::GetParentMatrix() {
    return m_model->m_parentMatrix;
}

void ModelBase::SetParentMatrix(const Mtx34f *matrix) {
    m_model->m_parentMatrix = matrix;
}

const Mtx34f *ModelBase::GetNodeMatrix(int index) {
    return m_model->GetNodeMatrix(index);
}

u32 *ModelBase::GetModelFlags() {
    return &m_model->m_flags;
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss
