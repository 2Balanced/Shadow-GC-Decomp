/* SuctionPillar::SuctionPillar (class name from RTTI; its s_taskName is "SuctionPillar"): the Suction
 * Pillar gadget, a pillar (SUCTIONPILLAR.BIN) with two targets on its sides. A vacuum command
 * (type 0x14) starts it: it sets the shared switch state of its owner's switch id, rumbles the pad
 * and rises along its local Z axis at 25 units a second (with a looping sound) until it has risen
 * s_maxHeight, then stops with a sound. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Sets the radius around the point (name inferred). */
    void SetRadius(const float &radius) {
        m_radius = radius;
    }

    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_radius;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);

/* GadgetModelInfo helpers (behaviour inferred; see BAMine.cpp and BASwitch.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Creates a Target for owner, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* Starts rumble pattern type for a character info, and stops a request (behaviour inferred; see
 * Rocket.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_80169B1C(int type, GadgetCharInfo *charInfo);
void fn_8034DB60(boost::weak_ptr<Vibration::RequestHandle> *handle);

/* Plays sound id at a character info, once and as a loop for source (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);

/* a + b (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by the static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* The targets' radius and how far the pillar rises (names inferred). */
static const float s_targetRadius = Span(40.0f);
static const float s_maxHeight = Span(80.0f);

namespace SuctionPillar {

/* The flag word at 0x68 (struct name unknown). */
struct SuctionPillarFlags {
    SuctionPillarFlags() {
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

    u32 m_bits; /* bit 0: started */
};

class SuctionPillar : public Gadget {
  public:
    SuctionPillar(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SuctionPillar();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and switch id and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the targets (name inferred). */
    void UpdatePosition();
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<Target> m_targets[2];
    /* 0x50 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x58 */ s32 m_state; /* 0 new, 1 waiting or rising, 2 done */
    /* 0x5C */ Vec3f m_origin;
    /* 0x68 */ SuctionPillarFlags m_flags;
    /* 0x6C */ float m_height;
    /* 0x70 */ s32 m_switchId;
    /* 0x74 */ boost::weak_ptr<Vibration::RequestHandle> m_vibration;
    /* 0x7C */ u32 pad_0x7C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(SuctionPillar, 0x80);

const char *SuctionPillar::s_fileName = "SUCTIONPILLAR.BIN";
const char *SuctionPillar::s_taskName = "SuctionPillar";

SuctionPillar::SuctionPillar(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x90, 4);
    fn_8015CF7C(m_chara.px);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SuctionPillar::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.px, 0, m_chara);
    for (int i = 0; i < 2; i++) {
        m_targets[i] = fn_801E7128(m_chara, 0x80);
        m_targets[i]->SetRadius(s_targetRadius);
    }
    if (m_model.px != 0) {
        m_particle = fn_80166AF4();
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particle;
        fn_8015FA78(m_model.px, fn_80169D00, &ref);
    }
    if (m_particle.px != 0) {
        fn_80167010(m_particle.px, -1);
    }
    UpdatePosition();
}

SuctionPillar::~SuctionPillar() {
    for (int i = 0; i < 2; i++) {
        fn_801E7108(&m_targets[i]);
    }
    fn_8034DB60(&m_vibration);
}

void SuctionPillar::Behave(float dt) {
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
        if (m_flags.Test(0)) {
            m_height += 25.0f * dt;
            if (m_height > s_maxHeight) {
                m_height = s_maxHeight;
                m_state = 2;
                fn_8034DB60(&m_vibration);
                fn_801D54F4(fn_80053164(), 0x793F, m_chara.px, 0, 1);
            } else {
                fn_801D53C4(fn_80053164(), 0x793E, m_chara.px, this, 3, 0, 2);
            }
            Mtx34f matrix;
            Vec3f offset(0.0f, 0.0f, m_height);
            fn_8040B6F8(&matrix);
            fn_8040B848(&matrix, m_chara->m_rotation.y);
            fn_8040B884(&matrix, m_chara->m_rotation.x);
            fn_8040B80C(&matrix, m_chara->m_rotation.z);
            /* The transformed offset is passed by address as a temporary (a named copy would add a
             * copy; GC/2.7 warns "not an lvalue" here). */
            AssignVector(m_chara.px->m_position,
                         fn_8040CDD0(&m_origin,
                                     &(const Vec3f &)fn_8040B67C(&matrix, &offset)));
            UpdatePosition();
        }
        break;
    case 2:
        break;
    }
}

void SuctionPillar::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePosition();
    if (m_model.px != 0) {
        Mtx34f matrix;
        u8 segment[0x18];
        Vec3f start(0.0f, 60.0f, 40.0f);
        Vec3f end(0.0f, 60.0f, s_maxHeight);
        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        AssignVector(start, fn_8040B67C(&matrix, &start));
        AssignVector(end, fn_8040B67C(&matrix, &end));
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
        m_model->fn_8016108C();
    }
}

void SuctionPillar::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    fn_800091D0(&m_origin, &m_chara->m_position);
    m_flags.Clear();
    m_height = 0.0f;
}

void SuctionPillar::UpdatePosition() {
    if (m_model.px != 0 && m_targets != 0) {
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
        Vec3f front(0.0f, 40.0f, 40.0f);
        AssignVector(front, fn_8040B67C(&matrix, &front));
        m_targets[0]->fn_800CCE38(&front);
        Vec3f back(0.0f, 40.0f, -40.0f);
        AssignVector(back, fn_8040B67C(&matrix, &back));
        m_targets[1]->fn_800CCE38(&back);
    }
}

void SuctionPillar::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14:
        if (!m_flags.Test(0)) {
            m_flags.Set(0);
            if (m_switchId > 0) {
                fn_801690D0(fn_800CB69C(), m_switchId, 1);
            }
            m_vibration = fn_80169B1C(8, m_chara.px);
        }
        break;
    }
}

} // namespace SuctionPillar

extern "C" void fn_80304120(void) {
    fn_8006B474(SuctionPillar::SuctionPillar::s_fileName, 1);
}

extern "C" void fn_8030411C(void) {}

namespace SuctionPillar {

void SuctionPillar::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SuctionPillar(parent, owner);
}

} // namespace SuctionPillar

GadgetClassDesc lbl_8055AF34 = {
    "SUCTION PILLAR", fn_80304120, fn_8030411C, SuctionPillar::SuctionPillar::Instantiate, 0, 0, 0x1902,
    1, 0, 10, 0,
};
