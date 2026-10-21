/* The effects that follow the frames of stage clumps (class, struct and file names unknown; all
 * names are inferred). Attaching a clump walks its frames and starts an effect at every frame whose
 * id is in s_frameEffects; the effect follows a matrix of its own that Tick copies from the
 * frame's LTM each frame. Up to 128 effects are kept; Clear stops them all. The manager is an empty
 * singleton (its getter is 0x8016F990); everything it keeps is static. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The RenderWare types used here, declared locally (as in HeavyDog0410CtrlUnit.cpp). */
extern "C" {
typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame RwFrame;
typedef struct RwMatrix RwMatrix;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);

RwMatrix *RwFrameGetLTM(RwFrame *frame);
/* RwFrameForAllChildren (behaviour inferred). */
RwFrame *fn_8048752C(RwFrame *frame, RwFrameCallBack callback, void *data);
/* The id of a frame (behaviour inferred). */
s32 fn_8044758C(RwFrame *frame);

/* A RenderWare matrix as the game's matrix (behaviour inferred). */
Mtx34f fn_800635A4(const RwMatrix *matrix);
/* Starts an effect that follows matrix (see EggLastMechSlotSwitch.cpp). */
boost::weak_ptr<EffectElement> fn_801D2C28(int arg0, void *matrix, int id, void *arg3);
}

/* Passes a returned matrix on by address (as in DevilDoomCtrlUnit.cpp). */
inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* The frame ids that get an effect, and the effect each gets. */
struct UnkFrameEffect {
    /* 0x0 */ s32 m_frameId;
    /* 0x4 */ s32 m_kind;
    /* 0x8 */ s32 m_effect;
};

static const UnkFrameEffect s_frameEffects[3] = {
    {25000, 0, 0x22},
    {25001, 0, 0x1B},
    {25002, 0, 0x1F},
};

/* An effect and the frame matrix it follows. */
struct UnkFrameEffectEntry {
    /* 0x0 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x8 */ RwMatrix *m_ltm;
    /* 0xC */ Mtx34f *m_matrix;
};

static UnkFrameEffectEntry s_entries[128];

/* The number of effects the last Attach started. */
static s32 s_count;

class UnkFrameEffects_803284B4 {
  public:
    UnkFrameEffects_803284B4();
    void Start();
    void End();
    void Clear();
    s32 Attach(RpClump *clump);
    void Tick();

    static RwFrame *AttachFrame(RwFrame *frame, void *data);
    static bool Add(const boost::weak_ptr<EffectElement> &effect, RwMatrix *ltm, Mtx34f *matrix);
};

UnkFrameEffects_803284B4::UnkFrameEffects_803284B4() {
    for (int i = 0; i < 128; i++) {
        s_entries[i].m_matrix = 0;
    }
}

void UnkFrameEffects_803284B4::End() {
    Clear();
}

void UnkFrameEffects_803284B4::Start() {
    Clear();
}

void UnkFrameEffects_803284B4::Clear() {
    for (int i = 0; i < 128; i++) {
        boost::shared_ptr<EffectElement> effect(s_entries[i].m_effect.lock());
        if (effect) {
            effect->Stop();
            effect->Kill();
            effect->Retire();
        }
        s_entries[i].m_effect.reset();
        if (s_entries[i].m_matrix != 0) {
            delete s_entries[i].m_matrix;
            s_entries[i].m_matrix = 0;
        }
    }
}

s32 UnkFrameEffects_803284B4::Attach(RpClump *clump) {
    s_count = 0;
    fn_8048752C((RwFrame *)clump->object.parent, AttachFrame, 0);
    return s_count;
}

void UnkFrameEffects_803284B4::Tick() {
    for (int i = 0; i < 128; i++) {
        boost::shared_ptr<EffectElement> effect(s_entries[i].m_effect.lock());
        if (effect && s_entries[i].m_matrix != 0 && s_entries[i].m_ltm != 0) {
            fn_8040C148(s_entries[i].m_matrix, &RefM(fn_800635A4(s_entries[i].m_ltm)));
        }
    }
}

RwFrame *UnkFrameEffects_803284B4::AttachFrame(RwFrame *frame, void *data) {
    Mtx34f *matrix;
    const UnkFrameEffect *entry;
    for (u32 i = 0; i < 3; i++) {
        /* The first id read is dropped; the assignment in the test gives retail's registers. */
        fn_8044758C(frame);
        if ((entry = &s_frameEffects[i])->m_frameId == fn_8044758C(frame)) {
            RwMatrix *ltm = RwFrameGetLTM(frame);
            matrix = new Mtx34f;
            fn_8040C148(matrix, &RefM(fn_800635A4(ltm)));
            Add(fn_801D2C28(entry->m_kind, matrix, entry->m_effect, 0), ltm, matrix);
            s_count++;
        }
    }
    return frame;
}

bool UnkFrameEffects_803284B4::Add(const boost::weak_ptr<EffectElement> &effect, RwMatrix *ltm,
                                   Mtx34f *matrix) {
    for (int i = 0; i < 128; i++) {
        boost::shared_ptr<EffectElement> current(s_entries[i].m_effect.lock());
        if (!current) {
            s_entries[i].m_effect = effect;
            s_entries[i].m_ltm = ltm;
            s_entries[i].m_matrix = matrix;
            return true;
        }
    }
    return false;
}
