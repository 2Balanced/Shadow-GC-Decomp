/* Door::SetDoor and Door::SetMorphDoor (class names from RTTI): the DOOR and MORPHDOOR gadgets,
 * which share one parameter table. A door is a model variant of DOOR.BIN picked by "model", with a
 * box collider of "width" x "height" x "depth" placed "X"/"Y"/"Z" from it. A "type : key" door
 * waits for the switch its owner names, a "type : normal" door opens while a character stands in
 * its collider and closes once it is empty again, with particles and the variant's "_open",
 * "_openend", "_closestart" and "_close" sounds. A morph door draws a morphing RenderWare clump
 * (PR0302DRAPE.DFF with PR0302DRAPE.DMA) through a Render callback and plays the morph instead of
 * the model's motion. File name inferred from the namespace. Method names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstring>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwSphere {
    RwV3d center;
    float radius;
} RwSphere;

typedef struct RwFrame RwFrame;
typedef struct RwCamera RwCamera;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RwGlobals {
    /* 0x000 */ RwCamera *curCamera;
} RwGlobals;

/* the engine instance */
extern RwGlobals *lbl_805F15B4;

/* whether a sphere is in a camera's view frustum (inferred) */
int fn_804856C8(const RwCamera *camera, const RwSphere *sphere);
/* translates a frame */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
/* Renders a clump. */
RpClump *fn_80456BC4(RpClump *clump);
/* Turn a frame about its z, x and y axes (behaviour inferred; the last argument is the combine
 * mode). */
void fn_80109AF8(RwFrame *frame, float angle, int combine);
void fn_800E9C64(RwFrame *frame, float angle, int combine);
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
/* Attaches the morph animation anim to clump, and shows its frame at time t, 0 to 1 (behaviour
 * inferred). */
void fn_80037A80(RpClump *clump, void *anim);
void fn_800379E8(RpClump *clump, float t);
}

/* A model's table of named entries in its resource file (names as in Stick). */
class UnkModelTable_80208888;

/* A sound entry of the table: fn_80208888 finds it by name (layout as in Stick and CircusBall). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 soundId; /* passed to the sound calls as 16 bits */
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

/* SetTransparentCommand (class name from RTTI): fades a character's model; its command type is
 * 0x17 (as in ContainerUnbreak; only the members used here are named). */
class SetTransparentCommand : public CharCommand {
  public:
    /* 0x14 */ u8 field_0x14;
    /* 0x18 */ float m_alpha; /* 1 or more: opaque (inferred) */
    /* 0x1C */ u32 field_0x1C;
};
SIZE_ASSERT(SetTransparentCommand, 0x20);

extern "C" {
/* Returns the file loader, and a file of it: the model table of DOOR.BIN, a clump or a morph
 * animation (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);
/* Returns the sound entry named name or 0 (behaviour inferred). */
UnkSoundEntry_80208888 *fn_80208888(UnkModelTable_80208888 *table, const char *name);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void fn_801D54F4(void *sound, int id, const Vec3f *position, int arg3, int arg4);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Loads the model file fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo helpers (behaviour inferred): the number of variants, the name of the variant
 * shown, attaching a character info to the collision of variant index, playing motion number
 * motion of a variant and whether its motion is over, fading the model to alpha and making it
 * opaque again, and the collider of a variant. fn_80161230 and fn_8016111C act on a variant when
 * the morph door opens and closes (behaviour unknown). */
int fn_8015FF44(GadgetModelInfo *model);
const char *fn_801626D8(GadgetModelInfo *model);
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
bool fn_80160040(GadgetModelInfo *model, int index);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);

/* The collider manager, a box collider of the given size for charInfo, and the removal of a
 * collider (behaviour inferred; the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4,
                                              int arg5, const Vec3f *offset,
                                              const Vec3f *rotation, int arg8);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* True when a character stands in the collision event's contacts (behaviour inferred). */
bool fn_8015AC84(ColliDetectArgs &args);

/* Particle controller helpers (behaviour inferred): create one, advance it by dt, and start or
 * stop its emitters. */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_80166D14(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
}

/* The contacts of a collider (the object holds them 0xC bytes in; name inferred). */
inline UnkColliContacts_8001EE04 *GetContacts(CharaColliBase *collider) {
    return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
}

namespace Door {

/* The parameters the owner supplies (names from the parameter table). */
struct DoorParams {
    DoorParams()
        : type(0), model(0), width(100.0f), height(100.0f), depth(100.0f), x(0.0f), y(0.0f),
          z(0.0f) {}

    /* 0x00 */ s32 type; /* 0 normal, 1 key */
    /* 0x04 */ s32 model;
    /* 0x08 */ float width;
    /* 0x0C */ float height;
    /* 0x10 */ float depth;
    /* 0x14 */ float x;
    /* 0x18 */ float y;
    /* 0x1C */ float z;
};
SIZE_ASSERT(DoorParams, 0x20);

class SetDoor : public Gadget {
  public:
    SetDoor(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetDoor();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    /* The door's state machine and the model's motion, sounds and variant (names inferred). */
    virtual void SetState(int state);
    virtual void PlayMotion(int motion);
    virtual bool IsMotionEnd();
    virtual void ChangeModel(int index);
    virtual void PlaySound(int sound);

    /* Creates the door under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Loads the model and the sound table, and builds the collider (name inferred). */
    void Init();
    /* The command unit's callback: fades the model (name inferred). */
    void OnCommand(CharCommand &command);
    /* True when a character is in the collider (name inferred). */
    bool IsCharacterIn();
    /* Copies the owner's position, rotation, parameters and switch id, and resets the state (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<CharaColliBase> m_collision;
    /* 0x48 */ DoorParams m_params;
    /* 0x68 */ s32 m_modelIndex; /* the variant the model shows */
    /* 0x6C */ s32 m_state;      /* 0 setup, 1 locked, 2 closed, 3 open, 4 unlocked, 5 opening,
                                  * 6 closing (inferred) */
    /* 0x70 */ UnkModelTable_80208888 *m_table;
    /* 0x74 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x7C */ u8 m_switchId; /* the switch a key door waits for */
};

class SetMorphDoor : public SetDoor {
  public:
    SetMorphDoor(Task *parent, UnkPlacement_800C9CD0 *owner, const char *clumpFile,
                 const char *animFile, float morphTime, float radius);
    virtual ~SetMorphDoor();
    virtual void Behave(float dt);
    virtual void SetState(int state);
    virtual void PlayMotion(int motion);
    virtual bool IsMotionEnd();
    /* Empty overrides, defined in the class: retail emits them first in the unit, ahead of
     * Instantiate. */
    virtual void ChangeModel(int index) {}
    virtual void PlaySound(int sound) {}

    /* Creates the door under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Loads the clump and its morph, registers the draw callback, and builds the collider (name
     * inferred). */
    void Init();
    /* The draw callback: draws the clump at the character info when it is in view (name
     * inferred). */
    void Draw(RednerDrawArgs &args);

    /* The clump and morph animation files (names inferred). */
    static const char *s_clumpFile;
    static const char *s_animFile;

    /* 0x80 */ const char *m_clumpFile;
    /* 0x84 */ const char *m_animFile;
    /* 0x88 */ float m_morphTime; /* seconds for a full morph */
    /* 0x8C */ float m_radius;    /* of the view test */
    /* 0x90 */ RpClump *m_clump;
    /* 0x94 */ boost::shared_ptr<Render> m_render;
    /* 0x9C */ s32 m_direction; /* 1 opening, 2 closing (the motion number) */
    /* 0xA0 */ s32 m_morphDone;
    /* 0xA4 */ float m_morphTimer;
};

} // namespace Door

SIZE_ASSERT(Door::SetDoor, 0x80);
SIZE_ASSERT(Door::SetMorphDoor, 0xA8);

namespace Door {

const char *SetDoor::s_taskName = "SetDoor";

SetDoor::SetDoor(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x22, 4);
}

SetDoor::~SetDoor() {
    if (m_collision.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collision);
    }
}

void SetDoor::Init() {
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SetDoor::OnCommand, this, _1)));
    LoadFromOwner(m_owner);
    m_table = static_cast<UnkModelTable_80208888 *>(fn_80012700(fn_800122D4(), 1, "DOOR.BIN"));
    m_modelIndex = m_params.model;
    m_model = fn_8015F800(1, "DOOR.BIN");
    m_model->fn_80162A68(m_modelIndex, 1);
    m_model->fn_80160C28(m_modelIndex);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.get(), m_modelIndex, m_chara, m_modelIndex);

    Vec3f offset(m_params.x, m_params.y + m_params.height, m_params.z);
    Vec3f size(m_params.width, m_params.height, m_params.depth);
    m_collision = fn_800C7C34(fn_80021B08(), m_chara, &size, 0x40000, 10, 0, &offset,
                              &lbl_805E07B8, 0);
    LoadFromOwner(m_owner);
    RebuildTransform();
}

void SetDoor::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }

    switch (m_state) {
    case 0:
        if (m_params.type == 1) {
            SetState(1);
        } else {
            SetState(2);
        }
        m_particle.reset();
        m_particle = fn_80166AF4();
        {
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particle.get() != 0) {
            if (m_state == 1) {
                fn_80167010(m_particle.get(), 0x1F6);
            } else {
                fn_80167010(m_particle.get(), 0x1F5);
            }
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            SetState(4);
            PlayMotion(1);
            fn_80166D14(m_particle.get());
            fn_80167010(m_particle.get(), 0x1F5);
            fn_80167448(m_particle.get(), 0x1F8);
        }
        break;
    case 2:
        if (IsCharacterIn()) {
            SetState(5);
            PlayMotion(1);
            PlaySound(1);
            fn_80167448(m_particle.get(), 0x1F8);
        }
        break;
    case 5:
        if (IsMotionEnd()) {
            SetState(3);
            PlaySound(3);
        }
        break;
    case 3:
        if (!IsCharacterIn()) {
            SetState(6);
            PlayMotion(2);
            PlaySound(4);
            fn_80167448(m_particle.get(), 0x1F7);
        }
        break;
    case 6:
        if (IsMotionEnd()) {
            PlaySound(2);
            SetState(2);
        }
        break;
    }

    RebuildTransform();
}

void SetDoor::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        if (m_params.model != m_modelIndex) {
            int count = fn_8015FF44(m_model.get());

            m_modelIndex = m_params.model;
            if (m_modelIndex < 0) {
                m_modelIndex = 0;
            }
            if (m_modelIndex > count - 1) {
                m_modelIndex = count - 1;
            }
            ChangeModel(m_modelIndex);
            m_params.model = m_modelIndex;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
    if (m_collision.get() != 0) {
        Vec3f offset(m_params.x, m_params.y + m_params.height, m_params.z);
        m_collision->SetOffset(&offset, -2);
        m_collision->SetSize(m_params.width, m_params.height, m_params.depth, -2);
        m_collision->Reset();
    }
}

void SetDoor::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_switchId);
    m_state = 0;
}

void SetDoor::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

bool SetDoor::IsCharacterIn() {
    if (m_collision.get() != 0) {
        UnkColliContacts_8001EE04 *contacts = GetContacts(m_collision.get());
        if (contacts != 0 && contacts->m_count > 0) {
            ColliDetectArgs args;
            args.m_contacts = contacts;
            return fn_8015AC84(args);
        }
    }
    return false;
}

/* The collider attribute set while the model is faded (name and meaning inferred). Retail loads it
 * from .sdata2 when setting it, which a namespace-scope const reproduces, but clears it with an
 * immediate mask. */
const u32 kTransparentAttribute = 0x100;

/* The attributes are read in their own statement: retail rereads the collider pointer after that
 * call instead of keeping it in a register. */
void SetDoor::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x17: /* SetTransparentCommand */
        if (static_cast<SetTransparentCommand &>(command).m_alpha < 1.0f) {
            if (m_model.get() != 0) {
                fn_8015FDD8(m_model.get(), static_cast<SetTransparentCommand &>(command).m_alpha);
                boost::shared_ptr<CharaColliBase> collider =
                    fn_80161614(m_model.get(), m_modelIndex).lock();
                if (collider.get() != 0) {
                    u32 attributes = collider->GetAttributes(0);
                    attributes |= kTransparentAttribute;
                    collider->SetAttributes(attributes, -1);
                }
            }
        } else {
            if (m_model.get() != 0) {
                fn_8015FD10(m_model.get());
                boost::shared_ptr<CharaColliBase> collider =
                    fn_80161614(m_model.get(), m_modelIndex).lock();
                if (collider.get() != 0) {
                    u32 attributes = collider->GetAttributes(0);
                    collider->SetAttributes(attributes & ~0x100, -1);
                }
            }
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void SetDoor::PlaySound(int sound) {
    switch (sound) {
    case 1: {
        const char *modelName = fn_801626D8(m_model.get());
        char name[0x40];
        memset(name, 0, sizeof(name));
        strcpy(name, modelName);
        strcat(name, "_open");
        UnkSoundEntry_80208888 *entry = fn_80208888(m_table, name);
        if (entry != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.x += entry->offset.x;
            position.y += entry->offset.y;
            position.z += entry->offset.z;
            fn_801D54F4(fn_80053164(), (u16)entry->soundId, &position, 0, 1);
        }
        break;
    }
    case 2: {
        const char *modelName = fn_801626D8(m_model.get());
        char name[0x40];
        memset(name, 0, sizeof(name));
        strcpy(name, modelName);
        strcat(name, "_close");
        UnkSoundEntry_80208888 *entry = fn_80208888(m_table, name);
        if (entry != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.x += entry->offset.x;
            position.y += entry->offset.y;
            position.z += entry->offset.z;
            fn_801D54F4(fn_80053164(), (u16)entry->soundId, &position, 0, 1);
        }
        break;
    }
    case 3: {
        const char *modelName = fn_801626D8(m_model.get());
        char name[0x40];
        memset(name, 0, sizeof(name));
        strcpy(name, modelName);
        strcat(name, "_openend");
        UnkSoundEntry_80208888 *entry = fn_80208888(m_table, name);
        if (entry != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.x += entry->offset.x;
            position.y += entry->offset.y;
            position.z += entry->offset.z;
            fn_801D54F4(fn_80053164(), (u16)entry->soundId, &position, 0, 1);
        }
        break;
    }
    case 4: {
        const char *modelName = fn_801626D8(m_model.get());
        char name[0x40];
        memset(name, 0, sizeof(name));
        strcpy(name, modelName);
        strcat(name, "_closestart");
        UnkSoundEntry_80208888 *entry = fn_80208888(m_table, name);
        if (entry != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.x += entry->offset.x;
            position.y += entry->offset.y;
            position.z += entry->offset.z;
            fn_801D54F4(fn_80053164(), (u16)entry->soundId, &position, 0, 1);
        }
        break;
    }
    }
}

void SetDoor::SetState(int state) {
    m_state = state;
}

void SetDoor::PlayMotion(int motion) {
    fn_80160A84(m_model.get(), m_modelIndex, motion);
}

bool SetDoor::IsMotionEnd() {
    return fn_80160040(m_model.get(), m_modelIndex);
}

void SetDoor::ChangeModel(int index) {
    m_model->fn_80161880();
    m_model->fn_801604FC();
    m_model->fn_80162804();
    m_model->fn_80162A68(index, 1);
    m_model->fn_80160C28(index);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.get(), index, m_chara, index);
}

} // namespace Door

extern "C" void fn_80143FE0(void) {
    fn_8006B474("DOOR.BIN", 1);
}

extern "C" void fn_80143FDC(void) {}

void Door::SetDoor::Instantiate(UnkPlacement_800C9CD0 *owner) {
    SetDoor *door = new SetDoor(fn_801EB304(fn_8001C4F8(), 11), owner);
    door->Init();
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct DoorParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The labels of the type values. */
static const char *lbl_805E624C[] = {
    "type : normal",
    "type : key",
};

/* The label callback of the type parameter. */
extern "C" const char *fn_80143F54(UnkLabelQuery_800CBC20 *cursor) {
    const DoorParamCursor *fields = (const DoorParamCursor *)cursor;

    if (fields->index == 0) {
        return lbl_805E624C[fields->value];
    }
    return 0;
}

static GadgetParamDesc lbl_8052986C[] = {
    {1, "type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80143F54, 0},
    {1, "model", 0, 0, 0x100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "width", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "height", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "depth", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805299D4 = {
    "DOOR", fn_80143FE0, fn_80143FDC, Door::SetDoor::Instantiate, 0, 0, 0x1133, 1, 0, 10, lbl_8052986C,
};

namespace Door {

SetMorphDoor::SetMorphDoor(Task *parent, UnkPlacement_800C9CD0 *owner, const char *clumpFile,
                           const char *animFile, float morphTime, float radius)
    : SetDoor(parent, owner), m_clumpFile(clumpFile), m_animFile(animFile), m_morphTime(morphTime),
      m_radius(radius), m_clump(0), m_direction(2), m_morphDone(1), m_morphTimer(0.0f) {}

SetMorphDoor::~SetMorphDoor() {}

void SetMorphDoor::Init() {
    LoadFromOwner(m_owner);
    m_table = static_cast<UnkModelTable_80208888 *>(fn_80012700(fn_800122D4(), 1, "MORPHDOOR.BIN"));
    m_modelIndex = m_params.model;
    m_clump = static_cast<RpClump *>(fn_80012700(fn_800122D4(), 1, m_clumpFile));
    void *anim = fn_80012700(fn_800122D4(), 1, m_animFile); /* a named local, as retail moves it */
    fn_80037A80(m_clump, anim);
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, "MorphDoor");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&SetMorphDoor::Draw, this, _1));
    m_model = fn_8015F800(1, "MORPHDOOR.BIN");
    fn_80161FC8(m_model.get(), m_modelIndex, m_chara);

    Vec3f offset(m_params.x, m_params.y + m_params.height, m_params.z);
    Vec3f size(m_params.width, m_params.height, m_params.depth);
    m_collision = fn_800C7C34(fn_80021B08(), m_chara, &size, 0x40000, 10, 0, &offset,
                              &lbl_805E07B8, 0);
    LoadFromOwner(m_owner);
    RebuildTransform();
}

void SetMorphDoor::Behave(float dt) {
    if (m_morphDone == 0) {
        if (m_direction == 1) {
            m_morphTimer += dt;
            float t = m_morphTimer / m_morphTime;
            fn_800379E8(m_clump, t);
            if (t > 1.0f) {
                m_morphDone = 1;
            }
        } else if (m_direction == 2) {
            m_morphTimer -= dt;
            float t = m_morphTimer / m_morphTime;
            fn_800379E8(m_clump, t);
            if (t < 0.0f) {
                m_morphDone = 1;
            }
        }
    }
    SetDoor::Behave(dt);
}

void SetMorphDoor::Draw(RednerDrawArgs &args) {
    RwSphere sphere;

    sphere.radius = m_radius;
    sphere.center.x = m_chara->m_position.x;
    sphere.center.y = m_chara->m_position.y;
    sphere.center.z = m_chara->m_position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwFrame *frame = (RwFrame *)m_clump->object.parent;

        fn_80109AF8(frame, m_chara->m_rotation.z, 0);
        fn_800E9C64(frame, m_chara->m_rotation.x, 2);
        fn_800E9CEC(frame, m_chara->m_rotation.y, 2);
        fn_804875B0(frame, &sphere.center, 2);
        fn_80456BC4(m_clump);
    }
}

void SetMorphDoor::PlayMotion(int motion) {
    m_direction = motion;
    m_morphDone = 0;
}

bool SetMorphDoor::IsMotionEnd() {
    return m_morphDone == 1;
}

void SetMorphDoor::SetState(int state) {
    switch (state) {
    case 5:
        fn_80161230(m_model.get(), m_modelIndex, -1);
        break;
    case 2:
        fn_8016111C(m_model.get(), m_modelIndex, -1);
        break;
    }
    m_state = state;
}

const char *SetMorphDoor::s_clumpFile = "PR0302DRAPE.DFF";
const char *SetMorphDoor::s_animFile = "PR0302DRAPE.DMA";

} // namespace Door

extern "C" void fn_80143504(void) {
    fn_8006B474("MORPHDOOR.BIN", 1);
    fn_8006B474(Door::SetMorphDoor::s_animFile, 0);
}

extern "C" void fn_80143500(void) {}

void Door::SetMorphDoor::Instantiate(UnkPlacement_800C9CD0 *owner) {
    SetMorphDoor *door = new SetMorphDoor(fn_801EB304(fn_8001C4F8(), 11), owner, s_clumpFile,
                                          s_animFile, 0.25f, 80.0f);
    door->Init();
}

GadgetClassDesc lbl_80529A04 = {
    "MORPHDOOR", fn_80143504, fn_80143500, Door::SetMorphDoor::Instantiate, 0, 0, 0xC89, 1, 0, 10,
    lbl_8052986C,
};
