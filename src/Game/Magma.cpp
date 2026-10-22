/* Magma::SetMagma (class name from RTTI; its s_taskName is "SetMagma"): the MAGMA gadget, a magma
 * surface (MAGMA.BIN, model variant "model") that waits for its switch and then rises (or, with a
 * negative "Y", sinks) at "speed" until it is "Y" above (below) where it was placed. Its model's
 * particles are started on the first frame. File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);

/* GadgetModelInfo helpers (behaviour inferred; see BAShield.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);
/* The character info's placement as a matrix (see BKTank.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* Particle controller helpers (behaviour inferred; see BAShield.cpp). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* vector += value (see BAMine.cpp). */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
}

namespace Magma {

/* The parameters of the owner record (names from the parameter table). */
struct SetMagmaParams {
    SetMagmaParams() : model(0), y(0.0f), speed(0.0f) {}

    /* 0x0 */ s32 model;
    /* 0x4 */ float y;
    /* 0x8 */ float speed;
};

class SetMagma : public Gadget {
  public:
    SetMagma(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetMagma();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement, parameters and switch id (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info (name inferred). */
    void UpdateModel();
    /* Moves towards the end height; true once it is there (name inferred). */
    bool Move(float dt);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ SetMagmaParams m_params;
    /* 0x54 */ s32 m_switchId;
    /* 0x58 */ s32 m_variant;
    /* 0x5C */ s32 m_state; /* 0 start, 1 waiting for the switch, 2 moving, 3 done */
    /* 0x60 */ Vec3f m_start;
    /* 0x6C */ u32 field_0x6C;
};
SIZE_ASSERT(SetMagma, 0x70);

const char *SetMagma::s_taskName = "SetMagma";

SetMagma::SetMagma(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x7C, 4);
    ReadPlacement(owner);
    m_variant = m_params.model;
    m_model = fn_8015F800(1, "MAGMA.BIN");
    m_model->fn_80162A68(m_variant, 0);
    fn_80161FC8(m_model.get(), m_variant, m_chara);
    UpdateModel();
}

SetMagma::~SetMagma() {}

void SetMagma::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        if (m_model.get() != 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 ref;
            ref.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &ref);
        }
        if (m_particle.get() != 0) {
            fn_80167010(m_particle.get(), -1);
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 2;
        }
        break;
    case 2:
        if (Move(dt)) {
            m_state = 3;
        }
        UpdateModel();
        break;
    case 3:
        break;
    }
}

void SetMagma::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 model = m_params.model;

    ReadPlacement(owner);
    UpdateModel();
    if (m_model.get() != 0 && model != m_params.model) {
        m_particle.reset();
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_variant = m_params.model;
        m_model->fn_80162A68(m_variant, 0);
        fn_80161FC8(m_model.get(), m_variant, m_chara);
    }
    Vec3f start;
    fn_800091EC(&start, &m_chara->m_position);
    Vec3f end(0.0f, m_params.y, 0.0f);
    fn_8040CFF8(&end, &start);
    u8 segment[0x18];
    fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
}

void SetMagma::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    fn_800091D0(&m_start, &m_chara->m_position);
    m_state = 0;
}

void SetMagma::UpdateModel() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

bool SetMagma::Move(float dt) {
    if (m_params.y < 0.0f) {
        float end = m_start.y + m_params.y;
        m_chara->m_position.y -= m_params.speed * dt;
        if (m_chara->m_position.y < end) {
            m_chara->m_position.y = end;
            return true;
        }
    } else {
        float end = m_start.y + m_params.y;
        m_chara->m_position.y += m_params.speed * dt;
        if (m_chara->m_position.y > end) {
            m_chara->m_position.y = end;
            return true;
        }
    }
    return false;
}

} // namespace Magma

using Magma::SetMagma;

extern "C" void fn_8027B968(void) {
    fn_8006B474("MAGMA.BIN", 1);
}

extern "C" void fn_8027B964(void) {}

void SetMagma::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetMagma(parent, owner);
}

static GadgetParamDesc lbl_8054DBC0[] = {
    {1, "model", 0, 0, 9, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "speed", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054DC60 = {
    "MAGMA", fn_8027B968, fn_8027B964, SetMagma::Instantiate, 0, 0, 0x1839, 1, 0, 10, lbl_8054DBC0,
};
