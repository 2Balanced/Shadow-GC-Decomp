/* SecretDoor (class SecretDoor::SecretDoor from RTTI; its s_taskName is "SecretDoor"): the SECRETDOOR
 * gadget, a door shown with the SECRETDOOR.BIN model and held shut by five lock parts. When the
 * player has collected more of the keys counted by fn_8011BE9C and stands close enough, one lock
 * part per half second is removed with an effect and a sound; once all five are gone the door opens
 * and the owner remembers it. File name inferred from the class. Names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class UnkSound_80053164;

/* The RenderWare types this unit touches, reduced to the members it uses (layouts from
 * rw/rwcore.h and rw/rpworld.h, which the game C++ units do not include). */
struct RwFrame;
struct RpWorld;

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
SIZE_ASSERT(RwObject, 0x8);

struct RpClump {
    /* 0x0 */ RwObject object; /* the parent is the clump's frame */
};

extern "C" {
/* Tests and sets bits of the owner's flag word at +0x18: fn_800C9E80 reports whether every bit of
 * mask is set (behaviour inferred). */
bool fn_800C9E80(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 mask);

/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and
 * fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Builds the character info of a gadget (behaviour inferred; arguments unknown). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Builds the matrix of the character info's position and rotation (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo methods that are not declared in its header yet (meanings unknown). */
void fn_80161A24(GadgetModelInfo *model, int arg1);
bool fn_80160040(GadgetModelInfo *model, int arg1);

/* Launches effect id at matrix (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D754(const Mtx34f *matrix, int id);
/* Returns the sound manager; fn_801D560C plays sound id and fn_801D53C4 plays it from source
 * (behaviour inferred; the other arguments are unknown). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D560C(UnkSound_80053164 *sound, int id, int arg2, int arg3);
void fn_801D53C4(UnkSound_80053164 *sound, int id, int arg2, void *source, int arg4, int arg5,
                 int arg6);

/* Returns the character info of the given id from the manager's list (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* The id of the player's character info (inferred). */
extern int lbl_805EE588;

/* Returns how many of the five secret keys have been collected (behaviour inferred). */
u32 fn_8011BE9C(void);

RwMatrix *RwFrameGetLTM(RwFrame *frame);
/* Returns the clump's world, and removes a clump from a world (see rw/rpworld.h). */
RpWorld *fn_8046256C(const RpClump *clump);
RpWorld *fn_804623EC(RpWorld *world, RpClump *clump);
}

/* One part of a model, as passed to the callback of fn_8015FA78 (layout from that function). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ RpClump *clump;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 field_0x8; /* nonzero for the lock parts (inferred) */
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

extern "C" {
/* Calls callback for each part of the model until it returns null (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model,
                 UnkModelPart_8015FA78 *(*callback)(UnkModelPart_8015FA78 *part, void *data),
                 void *data);
}

namespace SecretDoor {

/* The lock parts still to remove and whether to show their removal (names inferred). */
struct RemoveLocksData {
    RemoveLocksData() : count(0), showEffect(false) {}

    /* 0x0 */ int count;
    /* 0x4 */ bool showEffect;
};
SIZE_ASSERT(RemoveLocksData, 0x8);

class SecretDoor : public Gadget {
  public:
    SecretDoor(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SecretDoor();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the door under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and byte setting and restarts the door (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's matrix (name inferred). */
    void RebuildTransform();
    /* Removes up to count lock parts from the world, optionally with an effect and a sound (name
     * inferred). */
    void RemoveLocks(int count, bool showEffect);

    static const char *s_fileName; /* (name inferred) */
    static const char *s_taskName;

    /* Member names and meanings from 0x40 on are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::weak_ptr<CharInfo> m_player;
    /* 0x48 */ s32 m_state; /* 0 new, 1 waiting for keys, 2 removing locks, 3 opening, 4 open */
    /* 0x4C */ float m_timer;        /* time to the next lock removal */
    /* 0x50 */ s32 m_keys;           /* keys counted so far */
    /* 0x54 */ s32 m_locksToRemove;
    /* 0x58 */ u8 field_0x58;        /* copied from the owner */
    /* 0x5C */ u32 m_flags;          /* bit 0: play the opening sound */
};
SIZE_ASSERT(SecretDoor, 0x60);

const char *SecretDoor::s_fileName = "SECRETDOOR.BIN";
const char *SecretDoor::s_taskName = "SecretDoor";

/* Returns x squared (name inferred). */
inline float Square(float x) {
    return x * x;
}

/* The squared distance within which the player's arrival with new keys removes locks (name
 * inferred). Being initialised through a function call, it is set up at startup. */
static float s_openRangeSq = Square(100.0f);

/* Removes the part's clump from its world while locks remain to be removed (name inferred). */
extern "C" UnkModelPart_8015FA78 *fn_8011BC70(UnkModelPart_8015FA78 *part, void *param) {
    RemoveLocksData *data = (RemoveLocksData *)param;

    if (data->count > 0) {
        if (part->field_0x8 != 0) {
            RpWorld *world = fn_8046256C(part->clump);
            if (world != 0) {
                fn_804623EC(world, part->clump);
                if (data->showEffect) {
                    RwFrame *frame = (RwFrame *)part->clump->object.parent;
                    if (frame != 0) {
                        RwMatrix *ltm = RwFrameGetLTM(frame);
                        Mtx34f matrix;

                        matrix.values[0][0] = ltm->right.x;
                        matrix.values[0][1] = ltm->up.x;
                        matrix.values[0][2] = ltm->at.x;
                        matrix.values[0][3] = ltm->pos.x;
                        matrix.values[1][0] = ltm->right.y;
                        matrix.values[1][1] = ltm->up.y;
                        matrix.values[1][2] = ltm->at.y;
                        matrix.values[1][3] = ltm->pos.y;
                        matrix.values[2][0] = ltm->right.z;
                        matrix.values[2][1] = ltm->up.z;
                        matrix.values[2][2] = ltm->at.z;
                        matrix.values[2][3] = ltm->pos.z;
                        fn_8015D754(&matrix, 0x296);
                        fn_801D560C(fn_80053164(), 0xE018, 0, 0);
                    }
                }
                data->count--;
            }
        }
        return part;
    }
    return 0;
}

SecretDoor::SecretDoor(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_flags);
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x14, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(0, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col0", m_chara);
    m_model->fn_80161F08("col1", m_chara);
    m_model->fn_80160BA0("mtn");
    m_model->fn_8015FF60();
    RebuildTransform();
}

SecretDoor::~SecretDoor() {}

void SecretDoor::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (fn_80014E34(&m_flags, 0)) {
        fn_801D53C4(fn_80053164(), 0xE019, 0, this, 2, 0, 2);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        m_player = fn_800A8B74(fn_80074194(), lbl_805EE588);
        if (m_owner != 0 && fn_800C9E80(m_owner, 0x400000)) {
            RemoveLocks(5, false);
            fn_80161A24(m_model.px, 1);
            m_state = 3;
            m_model->fn_80160B18(0);
            fn_80014E1C(&m_flags, 0);
        }
        break;
    case 1: {
        u32 keys = fn_8011BE9C();
        if (keys != 0) {
            if (m_keys != keys) {
                if (!m_player.expired()) {
                    boost::shared_ptr<CharInfo> player = m_player.lock();
                    Vec3f offset;

                    fn_8040CD74(&offset, &m_chara->m_position, &player->m_position);
                    if (fn_8040CF2C(&offset) <= s_openRangeSq) {
                        m_state = 2;
                        m_timer = 0.5f;
                        m_locksToRemove = keys - m_keys;
                        m_keys = keys;
                    }
                }
            } else if (m_keys >= 5) {
                fn_80161A24(m_model.px, 1);
                m_state = 3;
                m_model->fn_80160B18(0);
                fn_80014E1C(&m_flags, 0);
                UnkPlacement_800C9CD0 *gadgetOwner = m_owner;
                if (gadgetOwner != 0) {
                    fn_800C9EB0(gadgetOwner, 0x400000);
                    fn_800C9EB0(gadgetOwner, 0x200);
                }
            }
        }
        break;
    }
    case 2:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_timer = 0.5f;
            if (m_locksToRemove <= 0) {
                m_state = 1;
            } else {
                RemoveLocks(1, true);
                m_locksToRemove--;
            }
        }
        break;
    case 3:
        if (fn_80160040(m_model.px, 0)) {
            fn_8006127C(&m_flags, 0);
            m_state = 4;
        }
        break;
    case 4:
        break;
    }

    RebuildTransform();
}

void SecretDoor::RemoveLocks(int count, bool showEffect) {
    RemoveLocksData data;

    data.count = count;
    data.showEffect = showEffect;
    fn_8015FA78(m_model.px, fn_8011BC70, &data);
}

void SecretDoor::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model.px != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void SecretDoor::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &field_0x58);
    m_keys = 0;
    m_timer = 0.0f;
    m_state = 0;
}

void SecretDoor::RebuildTransform() {
    if (m_model.px != 0) {
        Mtx34f matrix;

        fn_8015CEA0(m_chara.px, &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

} // namespace SecretDoor

extern "C" void fn_8011B62C(void) {
    fn_8006B474(SecretDoor::SecretDoor::s_fileName, 1);
}

extern "C" void fn_8011B628(void) {}

namespace SecretDoor {

void SecretDoor::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SecretDoor(parent, owner);
}

} // namespace SecretDoor

GadgetClassDesc lbl_80527158 = {
    "SECRETDOOR", fn_8011B62C, fn_8011B628, SecretDoor::SecretDoor::Instantiate, 0, 0, 0x1E, 1, 0, 10, 0,
};
