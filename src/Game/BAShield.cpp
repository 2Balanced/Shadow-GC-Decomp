/* BAShield::BAShield (class name from RTTI; its s_taskName is "BAShield"): the BA Shield gadget, a
 * barrier (BASHIELD.BIN, model variant "type") that follows a BA Switch. A shield of type 0 stands
 * while its switch is off and drops when the switch turns on; any other type does the opposite.
 * While it stands it hums. The switch states come from BASwitch.cpp (fn_80301F4C). File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);

/* GadgetModelInfo helpers (behaviour inferred; see BAGunShip.cpp and BAMine.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Plays sound id at a character info, once and as a loop for source (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);

/* Whether BA switch id is on (see BASwitch.cpp). */
bool fn_80301F4C(int id);
}

namespace BAShield {

/* The flag word at 0x58 (struct name unknown). */
struct BAShieldFlags {
    BAShieldFlags() {
        fn_80014E04(&m_bits);
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

    u32 m_bits; /* bit 0: the shield is down */
};

/* The parameters the owner supplies (names from the parameter table). */
struct BAShieldParams {
    BAShieldParams() : type(0) {}

    /* 0x0 */ s32 type;
};
SIZE_ASSERT(BAShieldParams, 0x4);

class BAShield : public Gadget {
  public:
    BAShield(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BAShield();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement, parameters and switch id and resets the state (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model (name inferred). */
    void UpdatePosition();
    /* Drops and raises the shield (names inferred). */
    void Lower();
    void Raise();
    /* Attaches a new particle controller to the model (name inferred). */
    void AttachParticle();

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ s32 m_state; /* 0 new, 1 running */
    /* 0x4C */ BAShieldParams m_params;
    /* 0x50 */ s32 m_variant; /* the model variant shown */
    /* 0x54 */ s32 m_switchId;
    /* 0x58 */ BAShieldFlags m_flags2;
    /* 0x5C */ u32 pad_0x5C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BAShield, 0x60);

const char *BAShield::s_fileName = "BASHIELD.BIN";
const char *BAShield::s_taskName = "BAShield";

BAShield::BAShield(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x8A, 4);
    ReadPlacement(owner);
    m_variant = m_params.type;
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(m_variant, 0);
    fn_80161FC8(m_model.px, m_variant, m_chara);
    AttachParticle();
    if (m_particle.px != 0) {
        fn_80167010(m_particle.px, 1);
    }
    UpdatePosition();
}

BAShield::~BAShield() {}

void BAShield::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.px != 0) {
        fn_8016761C(m_particle.px, dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (m_params.type == 0) {
            if (fn_80301F4C(m_switchId)) {
                if (!m_flags2.Test(0)) {
                    Lower();
                    m_flags2.Set(0);
                }
            } else {
                if (m_flags2.Test(0)) {
                    Raise();
                    m_flags2.Clear(0);
                }
                fn_801D53C4(fn_80053164(), 0x793D, m_chara.px, this, 3, 0, 2);
            }
        } else {
            if (fn_80301F4C(m_switchId)) {
                if (m_flags2.Test(0)) {
                    Raise();
                    m_flags2.Clear(0);
                }
                fn_801D53C4(fn_80053164(), 0x793D, m_chara.px, this, 3, 0, 2);
            } else {
                if (!m_flags2.Test(0)) {
                    Lower();
                    m_flags2.Set(0);
                }
            }
        }
        break;
    case 2:
        break;
    }
}

void BAShield::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 type = m_params.type;
    ReadPlacement(owner);
    UpdatePosition();
    if (m_model.px != 0) {
        if (type != m_params.type) {
            m_particle.reset();
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_variant = m_params.type;
            m_model->fn_80162A68(m_variant, 0);
            fn_80161FC8(m_model.px, m_variant, m_chara);
            AttachParticle();
            if (m_particle.px != 0) {
                fn_80167010(m_particle.px, 1);
            }
        }
        m_model->fn_8016108C();
    }
    if (m_particle.px != 0) {
        fn_8016761C(m_particle.px, 0.0f);
    }
}

void BAShield::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_flags2.Clear();
}

void BAShield::UpdatePosition() {
    if (m_model.px != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void BAShield::Lower() {
    if (m_particle.px != 0) {
        m_particle.reset();
        AttachParticle();
        fn_80167448(m_particle.px, 0);
    }
    if (m_model.px != 0) {
        fn_80161230(m_model.px, m_variant, -1);
    }
    fn_801D54F4(fn_80053164(), 0x793C, m_chara.px, 0, 1);
}

void BAShield::Raise() {
    if (m_particle.px != 0) {
        m_particle.reset();
        AttachParticle();
        fn_80167010(m_particle.px, 1);
    }
    if (m_model.px != 0) {
        fn_8016111C(m_model.px, m_variant, -1);
    }
    fn_801D54F4(fn_80053164(), 0x793B, m_chara.px, 0, 1);
}

void BAShield::AttachParticle() {
    if (m_model.px != 0) {
        m_particle = fn_80166AF4();
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particle;
        fn_8015FA78(m_model.px, fn_80169D00, &ref);
    }
}

} // namespace BAShield

extern "C" void fn_80301504(void) {
    fn_8006B474(BAShield::BAShield::s_fileName, 1);
}

extern "C" void fn_80301500(void) {}

namespace BAShield {

void BAShield::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BAShield(parent, owner);
}

} // namespace BAShield

static GadgetParamDesc lbl_8055ACA8[] = {
    {1, "type", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055ACF8 = {
    "BA Shield", fn_80301504, fn_80301500, BAShield::BAShield::Instantiate, 0, 0, 0x1901, 1, 0, 10,
    lbl_8055ACA8,
};
