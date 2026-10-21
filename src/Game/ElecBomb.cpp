/* ElecBomb (class name from RTTI): a bomb of the electric circuit area that shows the ELECBOMB.BIN
 * model. When a character comes within 150 units it switches the model's parts, plays a sound and
 * on the next frame explodes: it stops its particles, hits the first player, shakes the camera and
 * rumbles the pads. Its state survives the gadget being re-created through an ElecBombData kept by
 * the owner. File name inferred from the class. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Mission.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* SetAdapterState (class name from RTTI): the base of the states a gadget owner keeps for its
 * gadget. Its vtable is defined elsewhere; only the destructor is known. */
class SetAdapterState {
  public:
    virtual ~SetAdapterState() {}
};

/* ElecBombData (class name from RTTI): the bomb's state kept by the owner. */
class ElecBombData : public SetAdapterState {
  public:
    /* 0x4 */ s32 m_state; /* the bomb's m_state (inferred) */
};
SIZE_ASSERT(ElecBombData, 0x8);

/* The gadget owner (class unknown; Gadget.h only declares it). Only the adapter state at 0x14 is
 * used here (name inferred). */
class UnkPlacement_800C9CD0 {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ SetAdapterState *m_adapterState;
};

/* The positioned object fn_802B4254 creates; the bomb keeps it at its position (its class is
 * unknown; it may be a sound source). */
class UnkSound_802B4254;

class UnkPlayerManager_8006D6A8;
class UnkCameraManager_80009548;
class UnkSoundManager_80053164;

/* What fn_8015FA78 passes for each part of a model (layout from fn_8015FA78; names inferred). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ void *m_object; /* shown or hidden by fn_80263F38 */
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

extern "C" {
/* The part callback defined below. */
UnkModelPart_8015FA78 *fn_80263F38(UnkModelPart_8015FA78 *part, void *data);
/* Calls callback for each part of model while it returns non-null (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback, void *data);
/* Part callback that attaches the part to the particle controller data refers to (behaviour
 * inferred). */
UnkModelPart_8015FA78 *fn_80169D00(UnkModelPart_8015FA78 *part, void *data);
/* Show and hide a part's object (behaviour inferred). */
void fn_80038F94(void *object);
void fn_80038FC0(void *object);
/* Creates a particle controller (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
/* Stops the controller's particles of the given id, all for -1 (behaviour inferred). */
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
/* Creates the positioned object of the given kind (behaviour and arguments inferred). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
/* Set the object's range, a flag and its position (behaviour inferred). */
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
/* shared_ptr<UnkSound_802B4254>::reset (inferred from its body). */
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);
/* Stores state as the owner's adapter state, and sets flag 0x8 of the owner's record (behaviour
 * inferred). */
void fn_800C9930(UnkPlacement_800C9CD0 *owner, SetAdapterState *state);
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Returns the player manager and its entry index (behaviour inferred). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Returns the camera manager and shakes the camera (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D664(UnkCameraManager_80009548 *manager, int arg1, float arg2, float arg3,
                 float arg4);
/* Starts a pad rumble (behaviour and arguments inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE74(int pattern, int pad, float duration);
/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
/* Returns the sound manager and plays sound id through it (behaviour inferred). */
UnkSoundManager_80053164 *fn_80053164(void);
int fn_801D560C(UnkSoundManager_80053164 *manager, int id, int arg2, int arg3);
}

class ElecBomb : public Gadget {
  public:
    ElecBomb(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecBomb();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the bomb under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the positioned object at the character info's position (name
     * inferred). */
    void RebuildTransform();
    /* True when the nearest character info is within 150 units; also sends it message 0x35 (name
     * inferred). */
    bool IsWithinReach();

    /* The bomb's state kept by the owner (name inferred). */
    ElecBombData *GetData();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ GadgetModelInfo *m_model;
    /* 0x3C */ u32 field_0x3C;
    /* 0x40 */ s32 m_state; /* 0 armed, 1 triggered, 2 exploded (inferred) */
    /* 0x44 */ boost::shared_ptr<CharInfo> field_0x44;
    /* 0x4C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x54 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x5C */ u32 field_0x5C;
};
SIZE_ASSERT(ElecBomb, 0x60);

const char *ElecBomb::s_taskName = "ElecBomb";

inline ElecBombData *ElecBomb::GetData() {
    return dynamic_cast<ElecBombData *>(m_owner->m_adapterState);
}

ElecBomb::ElecBomb(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x56, 4, "ElecBomb");
    ReadPlacement(owner);
    if (owner->m_adapterState == 0) {
        ElecBombData *data = new ElecBombData;
        fn_800C9930(owner, data);
        data->m_state = 0;
        m_state = 0;
    } else {
        m_state = GetData()->m_state;
    }

    m_model = new GadgetModelInfo(1, "ELECBOMB.BIN");
    m_model->fn_80162A68(0, 0);
    if (m_state == 0) {
        if (m_model != 0) {
            m_particles = fn_80166AF4();
            UnkParticleAttach_80169D00 ref;
            ref.ctrl = m_particles;
            fn_8015FA78(m_model, fn_80169D00, &ref);
        }
        fn_8015FA78(m_model, fn_80263F38, (void *)4);
        fn_8015FA78(m_model, fn_80263F38, (void *)0x8003);
        m_sound = fn_802B4254(0, 0);
        fn_802B4498(m_sound.px, 1200.0f);
        fn_802B4510(m_sound.px, 1);
    } else {
        fn_8015FA78(m_model, fn_80263F38, (void *)3);
        fn_8015FA78(m_model, fn_80263F38, (void *)0x8004);
    }
    RebuildTransform();
}

ElecBomb::~ElecBomb() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecBomb::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (IsWithinReach()) {
            m_state = 1;
            fn_8015FA78(m_model, fn_80263F38, (void *)3);
            fn_8015FA78(m_model, fn_80263F38, (void *)0x8004);
            fn_801D560C(fn_80053164(), 0x7248, 0, 0);
        }
        break;
    case 1:
        fn_80167448(m_particles.px, -1);
        {
            boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 0).lock();
            if (player.get() != 0) {
                player->AddCount(1, 0);
            }
        }
        fn_800CC978(&m_sound);
        m_state = 2;
        GetData()->m_state = 2;
        fn_800C985C(m_owner);
        fn_8000D664(fn_80009548(), 0, 0.35f, 2.2f, 10.0f);
        fn_8034DE74(8, -2, 2.0f);
        break;
    case 2:
        break;
    }

    RebuildTransform();
}

void ElecBomb::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    m_state = 0;
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
    }
}

void ElecBomb::fn_800CBC1C() {
    if (m_model != 0) {
        fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, 150.0f);
    }
}

bool ElecBomb::IsWithinReach() {
    boost::shared_ptr<CharInfo> nearest;

    nearest = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
    if (nearest.get() != 0) {
        Vec3f offset;

        fn_8040CD74(&offset, &m_chara->m_position, &nearest->m_position);
        if (fn_8040CF2C(&offset) <= 22500.0f) {
            fn_8020642C(nearest, 0x35);
            return true;
        }
        return false;
    }

    return false;
}

void ElecBomb::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void ElecBomb::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }

    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, 170.0f, 0.0f);
        Vec3f position;

        fn_8040CDD0(&position, &offset, &m_chara->m_position);
        fn_802B44C8(m_sound.px, &position);
    }
}

/* Part callback: shows (data bit 15 set) or hides (bit 15 clear) the parts whose index bit is set
 * in data (behaviour inferred). */
extern "C" UnkModelPart_8015FA78 *fn_80263F38(UnkModelPart_8015FA78 *part, void *data) {
    u32 mask = (u32)data;

    if ((1 << part->m_index) & mask) {
        if (mask & 0x8000) {
            fn_80038F94(part->m_object);
        } else {
            fn_80038FC0(part->m_object);
        }
        return part;
    }
    return part;
}

extern "C" void fn_80263F0C(void) {
    fn_8006B474("ELECBOMB.BIN", 1);
}

extern "C" void fn_80263F08(void) {}

void ElecBomb::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecBomb(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_8054BFBC = {
    "ELEC_BOMB", fn_80263F0C, fn_80263F08, ElecBomb::Instantiate, 0, 0, 0x7DC, 1, 0, 10, 0,
};
