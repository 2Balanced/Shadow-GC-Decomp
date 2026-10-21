/* Door::SetBaseDoor (class name from RTTI; its s_taskName is "SetBaseDoor"): the BASE DOOR gadget,
 * a door of BASEDOOR.BIN shown as two models, variant "model" and variant "model" + 1000 (the
 * moving part), with a box collider of "width" x "height" x "depth" placed "X"/"Y"/"Z" from it, the
 * same parameters as Door::SetDoor (Game/Door.cpp). A "type : key" door waits for its switch, a
 * "type : normal" door opens while a character stands in its collider and closes once it is empty
 * again, with particles and sounds. File name inferred from the class (Game/Door.cpp holds the
 * other Door classes). Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

struct _stMtnPlayWork;

extern "C" {
/* A 32-bit flag word (see BAGunShip.cpp; fn_80014E10 clears it). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The switch id the owner was placed with (see BAGunShip.cpp). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);

/* GadgetModelInfo helpers (behaviour inferred; see Door.cpp and BAGunShip.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
bool fn_80160040(GadgetModelInfo *model, int index);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);
/* Sets the motion work's frame (behaviour inferred). */
void fn_80416ED0(_stMtnPlayWork *work, int arg1, float frame);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166B70(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80166C70(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The collider manager, a box collider of the given size for charInfo (see Door.cpp), and the
 * handle a collider is wrapped in, re-registered and advanced by dt (see BAGunShip.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4,
                                              int arg5, const Vec3f *offset,
                                              const Vec3f *rotation, int arg8);
extern Vec3f lbl_805E07B8;
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* True when a character stands in the collision event's contacts (see Door.cpp). */
bool fn_8015AC84(ColliDetectArgs &args);

/* Plays sound id at a character info, once and as a loop for source (see BAGunShip.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
}

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for the motion work (boost 1.32's definition; as in MessageTask.cpp). */
namespace boost {
template <> inline void weak_ptr<_stMtnPlayWork>::reset() {
    this_type().swap(*this);
}
} // namespace boost

namespace Door {

/* The parameters the owner supplies (names from the parameter table; as Door.cpp's DoorParams). */
struct BaseDoorParams {
    BaseDoorParams()
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
SIZE_ASSERT(BaseDoorParams, 0x20);

/* The flag word at 0x94 (struct name unknown). */
struct SetBaseDoorFlags {
    SetBaseDoorFlags() {
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

    u32 m_bits; /* bit 0: a character is in the collider; 1: the door moves (loop sound) */
};

class SetBaseDoor : public Gadget {
  public:
    SetBaseDoor(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetBaseDoor();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the door under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, parameters and switch id, and resets the state. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places both models at the character info, turned about the Y axis. */
    void RebuildTransform();
    /* The collider's callback. */
    void OnColliDetect(ColliDetectArgs &args);
    /* Loads variant index of the model and its moving part. */
    void LoadModel(int index);
    /* Drops the models and the motion. */
    void ReleaseModels();
    /* Switches the collisions of the closed (0) or open (1) door on. */
    void SetCollision(int open);

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_door; /* variant m_modelIndex + 1000 */
    /* 0x48 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x50 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x58 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_doorParticle;
    /* 0x60 */ BaseDoorParams m_params;
    /* 0x80 */ s32 m_state; /* 0 setup, 1 locked, 2 closed, 3 opening, 4 open, 6 closing */
    /* 0x84 */ s32 m_modelIndex;
    /* 0x88 */ s32 m_switchId;
    /* 0x8C */ boost::weak_ptr<_stMtnPlayWork> m_motion;
    /* 0x94 */ SetBaseDoorFlags m_flags;
};
SIZE_ASSERT(SetBaseDoor, 0x98);

const char *SetBaseDoor::s_fileName = "BASEDOOR.BIN";
const char *SetBaseDoor::s_taskName = "SetBaseDoor";

SetBaseDoor::SetBaseDoor(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(0x22, 4);
    m_modelIndex = m_params.model;
    LoadModel(m_modelIndex);
    SetCollision(0);
    {
        Vec3f offset(m_params.x, m_params.y + m_params.height, m_params.z);
        Vec3f size(m_params.width, m_params.height, m_params.depth);
        boost::shared_ptr<CharaColliBase> collider = fn_800C7C34(
            fn_80021B08(), m_chara, &size, 0x40000, 10, 0, &offset, &lbl_805E07B8, 0);
        m_handle = fn_8015B8C4(collider);
        fn_800F5980(m_handle.px, boost::bind(&SetBaseDoor::OnColliDetect, this, _1));
    }
    ReadPlacement(owner);
    RebuildTransform();
}

SetBaseDoor::~SetBaseDoor() {}

void SetBaseDoor::Behave(float dt) {
    m_flags.Clear(0);
    if (m_handle.px != 0) {
        fn_8015BE9C(m_handle.px, dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_door.get() != 0) {
        m_door->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
    if (m_flags.Test(1)) {
        fn_801D53C4(fn_80053164(), 0x7849, m_chara.get(), this, 3, 0, 2);
    }

    switch (m_state) {
    case 0:
        if (m_params.type == 1) {
            m_state = 1;
        } else {
            m_state = 2;
        }
        if (m_model.get() != 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particle.get() != 0) {
            fn_80167010(m_particle.get(), -1);
        }
        if (m_door.get() != 0) {
            m_doorParticle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_doorParticle;
            fn_8015FA78(m_door.get(), fn_80169D00, &attach);
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 5;
            SetCollision(1);
            if (m_door.get() != 0) {
                fn_80160A84(m_door.get(), m_modelIndex + 1000, 1);
            }
            if (m_doorParticle.get() != 0) {
                fn_80167448(m_doorParticle.get(), -1);
            }
            if (m_particle.get() != 0) {
                fn_80166C70(m_particle.get());
            }
            fn_801D54F4(fn_80053164(), 0x783C, m_chara.get(), 0, 1);
        }
        break;
    case 2:
        if (m_flags.Test(0)) {
            SetCollision(1);
            if (m_door.get() != 0) {
                fn_80160A84(m_door.get(), m_modelIndex + 1000, 1);
            }
            if (m_doorParticle.get() != 0) {
                fn_80167448(m_doorParticle.get(), -1);
            }
            if (m_particle.get() != 0) {
                fn_80166C70(m_particle.get());
            }
            m_state = 3;
            m_flags.Set(1);
        }
        break;
    case 3:
        if (fn_80160040(m_door.get(), m_modelIndex + 1000)) {
            m_state = 4;
            m_flags.Clear(1);
            fn_801D54F4(fn_80053164(), 0x7847, m_chara.get(), 0, 1);
        }
        break;
    case 4:
        if (!m_flags.Test(0)) {
            if (m_door.get() != 0) {
                fn_80160A84(m_door.get(), m_modelIndex + 1000, 2);
            }
            m_state = 6;
            m_flags.Set(1);
        }
        break;
    case 6:
        if (fn_80160040(m_door.get(), m_modelIndex + 1000)) {
            m_state = 2;
            SetCollision(0);
            boost::weak_ptr<_stMtnPlayWork> motion = fn_801602B4(m_model.get(), m_modelIndex);
            if (!motion.expired()) {
                if (boost::shared_ptr<_stMtnPlayWork> work = motion.lock()) {
                    fn_80416ED0(work.get(), 0, -1.0f);
                }
            }
            if (m_particle.get() != 0) {
                fn_80166B70(m_particle.get());
            }
            m_flags.Clear(1);
            fn_801D54F4(fn_80053164(), 0x7847, m_chara.get(), 0, 1);
        }
        break;
    }

    RebuildTransform();
}

void SetBaseDoor::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_params.model != m_modelIndex) {
        m_modelIndex = m_params.model;
        m_particle.reset();
        ReleaseModels();
        LoadModel(m_modelIndex);
        SetCollision(0);
        RebuildTransform();
    }
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
    if (m_handle.get() != 0) {
        Vec3f offset(m_params.x, m_params.y + m_params.height, m_params.z);
        m_handle->m_collider->SetOffset(&offset, -2);
        m_handle->m_collider->SetSize(m_params.width, m_params.height, m_params.depth, -2);
        fn_8015BE64(m_handle.get());
    }
}

void SetBaseDoor::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_flags.Clear();
}

void SetBaseDoor::RebuildTransform() {
    if (m_model.get() != 0 && m_door.get() != 0) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
        m_door->fn_801633DC(&matrix);
    }
}

void SetBaseDoor::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015AC84(args)) {
        m_flags.Set(0);
    }
}

void SetBaseDoor::LoadModel(int index) {
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(index, 0);
    if (m_model->fn_80160C28(index)) {
        m_model->fn_8015FF60();
        fn_80160A84(m_model.get(), index, 1);
    }
    fn_80161FC8(m_model.get(), index, m_chara);
    int doorIndex = index + 1000;
    m_door = fn_8015F800(1, s_fileName);
    m_door->fn_80162A68(doorIndex, 0);
    m_door->fn_80160C28(doorIndex);
    m_model->fn_8015FF60();
    fn_80161BD8(m_door.get(), doorIndex, m_chara, doorIndex);
    m_motion = fn_801602B4(m_door.get(), doorIndex);
}

void SetBaseDoor::ReleaseModels() {
    if (m_door.get() != 0) {
        m_door.reset();
    }
    if (m_model.get() != 0) {
        m_model.reset();
    }
    m_motion.reset();
}

void SetBaseDoor::SetCollision(int open) {
    switch (open) {
    case 0:
        if (m_model.get() != 0) {
            fn_8016271C(m_model.get());
            fn_8016111C(m_model.get(), m_modelIndex, -1);
        }
        if (m_door.get() != 0) {
            fn_80162790(m_door.get());
            fn_80161230(m_door.get(), m_modelIndex + 1000, -1);
        }
        break;
    case 1:
        if (m_model.get() != 0) {
            fn_80162790(m_model.get());
            fn_80161230(m_model.get(), m_modelIndex, -1);
        }
        if (m_door.get() != 0) {
            fn_8016271C(m_door.get());
            fn_8016111C(m_door.get(), m_modelIndex + 1000, -1);
        }
        break;
    }
}

} // namespace Door

/* The fields of the cursor that the label callback reads (layout from its accesses; as in
 * Door.cpp). */
struct BaseDoorParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The labels of the type values. */
static const char *s_typeLabels[] = {
    "type : normal",
    "type : key",
};

/* The label callback of the type parameter. */
extern "C" const char *fn_802566F8(UnkLabelQuery_800CBC20 *cursor) {
    const BaseDoorParamCursor *fields = (const BaseDoorParamCursor *)cursor;

    if (fields->index == 0) {
        return s_typeLabels[fields->value];
    }
    return 0;
}

extern "C" void fn_802566CC(void) {
    fn_8006B474("BASEDOOR.BIN", 1);
}

extern "C" void fn_802566C8(void) {}

void Door::SetBaseDoor::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetBaseDoor(parent, owner);
}

static GadgetParamDesc lbl_80549BDC[] = {
    {1, "type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_802566F8, 0},
    {1, "model", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "width", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "height", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "depth", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80549D44 = {
    "BASE DOOR", fn_802566CC, fn_802566C8, Door::SetBaseDoor::Instantiate, 0, 0, 0x13F1, 1, 0, 10,
    lbl_80549BDC,
};
