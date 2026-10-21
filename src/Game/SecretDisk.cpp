/* SecretDisk (class SecretDisk::SecretDisk from RTTI; its s_taskName is "SecretDisk"): the SECRET DISK
 * gadget, a disk (SECRETDISK.BIN) with particles and a positioned object above it. When a
 * character touches it, the player object of the manager at fn_8006D6A8 is hit, an effect starts
 * just below the disk, a sound plays and the disk reports itself collected and finishes. File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "Game/Mission.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The positioned object fn_802B4254 creates (see ElecBomb.cpp; it may be a sound source). */
class UnkSound_802B4254;

class UnkPlayerManager_8006D6A8;

extern "C" {
/* GadgetModelInfo helpers (see CityBombSmall.cpp and Door.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* The character info's matrix (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);

/* True when a character stands in the collision event's contacts (behaviour inferred). */
bool fn_8015AC84(ColliDetectArgs &args);
/* Starts effect id at position and rotation (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168B38(const Vec3f *position,
                                                           const Vec3f *rotation, int id);
/* Tells the owner that the gadget was collected (behaviour inferred). */
void fn_8015A768(Gadget *gadget);

/* The player manager and its entry (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
}

/* A length (name inferred). As a call it is not a constant expression, so the constant below is
 * stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The height of the positioned object above the disk (name inferred). */
static float s_soundHeight = Span(10.0f);

namespace SecretDisk {

class SecretDisk : public Gadget {
  public:
    SecretDisk(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SecretDisk();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the disk under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation and restarts the disk (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the positioned object (name inferred). */
    void UpdatePosition();
    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x48 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x50 */ s32 m_state; /* 0 new, 1 waiting, 2 touched */
    /* 0x54 */ u32 field_0x54; /* never used here */
};

} // namespace SecretDisk

SIZE_ASSERT(SecretDisk::SecretDisk, 0x58);

namespace SecretDisk {

const char *SecretDisk::s_fileName = "SECRETDISK.BIN";
const char *SecretDisk::s_taskName = "SecretDisk";

SecretDisk::SecretDisk(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x40, 4);
    LoadFromOwner(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&SecretDisk::OnColliDetect, this, _1));
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80160A84(m_model.get(), 0, 1);
    m_sound = fn_802B4254(2, 0);
    fn_802B4510(m_sound.px, 1);
    UpdatePosition();
}

SecretDisk::~SecretDisk() {}

void SecretDisk::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (m_model.px != 0) {
            m_particles = fn_80166AF4();
            {
                UnkParticleAttach_80169D00 ref;
                ref.ctrl = m_particles;
                fn_8015FA78(m_model.get(), fn_80169D00, &ref);
            }
        }
        if (m_particles.px != 0) {
            fn_80167010(m_particles.px, -1);
        }
        m_state = 1;
        break;
    case 1:
        break;
    case 2: {
        boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
        if (player.get() != 0) {
            player->AddCount(1, 0);
        }
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += -6.0f;
        fn_80168B38(&position, &m_chara->m_rotation, 0x33);
        fn_801D560C(fn_80053164(), 0xE017, 0, 0);
        fn_8015A768(this);
        m_flags |= 1;
        break;
    }
    }

    UpdatePosition();
}

void SecretDisk::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdatePosition();
    if (m_model.px != 0) {
        m_model->fn_80162EF4();
    }
}

void SecretDisk::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
}

void SecretDisk::UpdatePosition() {
    if (m_model.px != 0) {
        Mtx34f matrix;

        fn_8015CEA0(m_chara.px, &matrix);
        m_model->fn_801633DC(&matrix);
    }
    if (m_sound.px != 0) {
        Vec3f position;

        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_soundHeight;
        fn_802B44C8(m_sound.px, &position);
    }
}

void SecretDisk::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 1 && fn_8015AC84(args)) {
        m_state = 2;
    }
}

} // namespace SecretDisk

extern "C" void fn_802F9E44(void) {
    fn_8006B474(SecretDisk::SecretDisk::s_fileName, 1);
}

extern "C" void fn_802F9E40(void) {}

namespace SecretDisk {

void SecretDisk::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SecretDisk(parent, owner);
}

} // namespace SecretDisk

GadgetClassDesc lbl_8055A5BC = {
    "SECRET DISK", fn_802F9E44, fn_802F9E40, SecretDisk::SecretDisk::Instantiate, 0, 0, 0x1453, 1, 0, 10,
    0,
};
