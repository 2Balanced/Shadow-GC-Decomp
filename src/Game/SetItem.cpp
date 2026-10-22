/* SetItem (classes SetItem::SetItemBoxTask, SetItem::ItemBoxTask, SetItem::ItemBox and
 * SetItem::VacuumItemBox from RTTI): the ITEM_BOX gadget, an item box. ItemBox is the box itself:
 * a character info with a model (model file 0xD7), a clump that shows the item it holds (the
 * "item" parameter: Ring5, Ring10, Ring20, Barrier, ..., or Roulette, which cycles through the
 * others every half second) and a target. A character that touches or attacks it receives the item
 * (GiveItem); a vacuum pulls it in as a VacuumItemBox, which hands the item to the vacuum's owner.
 * SetItemBoxTask is the stage object; ItemBoxTask keeps a box that other code drops at run time
 * (fn_8010B824). File name inferred from the namespace. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/VacuumObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits its own copy (0x8010BD4C), so it defines it here (boost 1.32's definition; as in
 * SetWeapon.cpp). */
namespace boost {
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; as in HintRing.cpp). */
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

typedef struct RwCamera RwCamera;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame RwFrame;

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
 * mode; fn_80109AF8 is HintRing.cpp's). */
void fn_80109AF8(RwFrame *frame, float angle, int combine);
void fn_800E9C64(RwFrame *frame, float angle, int combine);
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
}

/* Target (class name from RTTI): a point characters aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
    /* Sets the target's range (name inferred). */
    void SetRange(float range) {
        m_reach = range;
    }
    /* Bit 0 of the target's flags: whether characters may aim at it (names and meaning
     * inferred). */
    bool IsEnabled() {
        return m_flags & 1;
    }
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_reach;
    /* 0x24 */ u8 field_0x24[0x30 - 0x24];
    /* 0x30 */ u32 m_flags;
};

/* The camera fn_8000D0F0 returns (class name unknown). */
namespace SonicteamUSA {
namespace System {
namespace Camera {
/* CameraManagerUnit (class name from the RTTI of its shared_ptr control block; see
 * Game/CameraManagerUnit.h). */
class CameraManagerUnit;
} // namespace Camera
} // namespace System
} // namespace SonicteamUSA
class UnkCameraManager_80009548;

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

extern "C" {
/* The flag word helpers (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* The model matrix of a character info (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* The attack collision of a model node (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *nodeName);
/* Creates a target for owner, and removes a gadget's target (behaviour inferred; the flags are
 * unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Scales a range (behaviour inferred). */
float fn_8005AE20(float range);
/* Creates a Render (behaviour inferred). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The file loader, and a clump of it by id (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, int arg1, int id);
/* Reports whether position is out of range (meaning inferred). */
bool fn_80169BE4(const Vec3f *position, float distanceSquared);
/* Gives target the item id (behaviour inferred; the meaning of the other arguments is
 * unknown). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Launches effect id at position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);
/* A test on a character (meaning unknown; see ElecFan.cpp). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &character);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
/* The character that pulls a vacuum object in (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_80238058(VacuumObject *object);
/* The camera manager, a camera of it, and a camera's angles (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<SonicteamUSA::System::Camera::CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
Vec3f fn_8000A610(const u8 *camera);
}

/* The angles of camera index (name inferred; as in HintRing.cpp). */
inline Vec3f GetCameraAngles(UnkCameraManager_80009548 *manager, int index) {
    return fn_8000A610(reinterpret_cast<const u8 *>(fn_8000D0F0(manager, index).get()) + 0x8);
}

/* Copies a rotation one component at a time (name inferred; Game/EnergyCore.cpp has the same
 * helper). The source is not const: the original reloads each component after the previous
 * store. */
inline void CopyRotation(Vec3f &vector, Vec3f &value) {
    vector.x = value.x;
    vector.y = value.y;
    vector.z = value.z;
}

/* The name of item (name inferred; defined below). */
inline const char *ItemName(int item);

namespace SetItem {

/* A 32-bit flag word (name inferred): fn_80014E04 clears it, fn_80014E10 clears all bits,
 * fn_80014E1C sets bit, fn_8006127C clears bit and fn_80014E34 tests bit. */
struct ItemBoxFlags {
    ItemBoxFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The box's model and state while a vacuum pulls it in (class name from RTTI; names inferred). */
class VacuumItemBox : public VacuumObject {
  public:
    VacuumItemBox(VacuumCommand &command, int item);
    virtual ~VacuumItemBox();
    virtual void Tick(float dt);
    virtual void fn_80238180();

    /* Starts a VacuumItemBox for item at position and rotation, and a task for it (name
     * inferred). */
    static void Start(VacuumCommand &command, int item, const Vec3f &position,
                      const Vec3f &rotation);

    /* 0x08 */ int m_item;
    /* 0x0C */ boost::shared_ptr<GadgetModelInfo> m_model;
};

/* What the hit functions pass to GiveItem (name inferred). */
struct ItemBoxHitContext {
    ItemBoxHitContext() : m_item(0), m_given(false) {}

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ int m_item;
    /* 0x10 */ bool m_given;
};

/* The box's clumps, placement, item and timer (struct name unknown; its constructor is out of
 * line). */
struct ItemBoxState {
    ItemBoxState()
        : m_clump(0), m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f), m_item(0),
          m_time(0.0f) {}

    /* 0x00 */ RpClump *m_clump; /* the item */
    /* 0x04 */ RpClump *m_box;   /* the box (clump 0x68) */
    /* 0x08 */ Vec3f m_position;
    /* 0x14 */ Vec3f m_rotation;
    /* 0x20 */ int m_item;
    /* 0x24 */ float m_time; /* until the roulette shows the next item */
};

/* The item box (class name from the RTTI of its shared_ptr control block). Flag bit 0: active;
 * bit 1: done; bit 2: the collision callbacks are set (names inferred). Method and member names
 * are inferred. */
class ItemBox {
  public:
    ItemBox(int item, float delay);
    ~ItemBox();

    void Tick(float dt);
    bool IsOutOfRange(float distanceSquared);
    void SetPlacement(const Vec3f &position, Vec3f &rotation);
    void UpdateMatrix();
    void OnColliDetect(ColliDetectArgs &args);
    void OnAttack(AttackCallbackParam &param);
    void LoadClump();
    void Draw(RednerDrawArgs &args);
    void OnCommand(CharCommand &command);

    /* True once the item was taken or the box vacuumed (name inferred). */
    bool IsDone() {
        return fn_80014E34(&m_flags.m_bits, 1);
    }

    /* 0x00 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x10 */ boost::shared_ptr<Render> m_render;
    /* 0x18 */ boost::shared_ptr<Target> m_target;
    /* 0x20 */ ItemBoxState m_state;
    /* 0x48 */ int m_param; /* the item it was created with: 10 is the roulette */
    /* 0x4C */ int m_mode;  /* 0 new, 1 waiting, 2 roulette, 3 taken, 4 vacuumed */
    /* 0x50 */ float m_delay; /* until the collision callbacks are set */
    /* 0x54 */ ItemBoxFlags m_flags;
};

/* Deletes a box (name inferred; see checked_delete below). */
inline void DeleteItemBox(ItemBox *box) {
    delete box;
}

} // namespace SetItem

/* Retail keeps boost::checked_delete<ItemBox> out of line (0x8010BACC) and ~ItemBox at its own
 * place (0x8010D224); this specialization calls an inline helper, which gives retail's code and
 * order (see docs/research-notes.md and Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<SetItem::ItemBox>(SetItem::ItemBox *x) {
    SetItem::DeleteItemBox(x);
}
} // namespace boost

namespace SetItem {

/* Keeps a box dropped at run time (class name from RTTI). */
class ItemBoxTask : public Task {
  public:
    ItemBoxTask(Task *parent, boost::shared_ptr<ItemBox> box);
    virtual ~ItemBoxTask();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<ItemBox> m_box;
};

/* The ITEM_BOX stage object (class name from RTTI). */
class SetItemBoxTask : public Gadget {
  public:
    SetItemBoxTask(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetItemBoxTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and item (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Moves the box to the character info (name inferred). */
    void PlaceBox();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<ItemBox> m_box;
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x40 */ s32 m_param; /* the "item" parameter */
    /* 0x44 */ s32 m_item;
};

} // namespace SetItem

SIZE_ASSERT(SetItem::VacuumItemBox, 0x14);
SIZE_ASSERT(SetItem::ItemBoxHitContext, 0x14);
SIZE_ASSERT(SetItem::ItemBoxState, 0x28);
SIZE_ASSERT(SetItem::ItemBox, 0x58);
SIZE_ASSERT(SetItem::ItemBoxTask, 0x30);
SIZE_ASSERT(SetItem::SetItemBoxTask, 0x48);

namespace SetItem {

const char *ItemBoxTask::s_taskName = "ItemBoxTask";
const char *SetItemBoxTask::s_taskName = "SetItemBoxTask";

/* Gives hit the context's item unless it was already given (name inferred). */
static void GiveItem(boost::shared_ptr<CharInfo> hit, void *user) {
    ItemBoxHitContext *context = static_cast<ItemBoxHitContext *>(user);

    if (!context->m_given) {
        static const int s_items[10] = {0, 0, 0, 1, 2, 3, 4, 9, 6, 5};
        static const int s_messages[10] = {4, 5, 6, 8, 8, 8, 8, 7, 8, 8};
        int count = 1;

        if (context->m_item == 0) {
            count = 5;
        } else if (context->m_item == 1) {
            count = 10;
        } else if (context->m_item == 2) {
            count = 20;
        }
        fn_8015F6EC(hit, s_items[context->m_item], count, 1);
        fn_8020642C(hit, s_messages[context->m_item]);
        fn_8015D794(&context->m_position, 0xA9);
        fn_801D560C(fn_80053164(), 0xE01C, 0, 0);
        if (hit->vf_0x24()) {
            if (!fn_800B7AD0(hit)) {
                Vec3f velocity(0.0f, 0.0f, 0.0f);
                hit->PushCommand(new Player::HomingJumpCommand(velocity, 4));
            }
        }
        context->m_given = true;
    }
}

ItemBox::ItemBox(int item, float delay) {
    m_param = item;
    m_state.m_item = m_param;
    m_chara = fn_8015CB88(0x17, 4, "ItemBox");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ItemBox::OnCommand, this, _1)));
    m_model = fn_8015F9D8(0, 0xD7);
    m_model->fn_80162A68(1000, 0);
    m_model->fn_80161F08("col", m_chara);
    m_render = fn_8004E24C(fn_80046178()->field_0x8);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ItemBox::Draw, this, _1));
    LoadClump();
    m_target = fn_801E7128(m_chara, 0xC90);
    m_target->SetRange(fn_8005AE20(5.0f));
    m_mode = 0;
    m_delay = delay;
    m_flags.Clear();
    m_flags.Set(0);
}

ItemBox::~ItemBox() {
    fn_801E7108(&m_target);
}

void ItemBox::Tick(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    m_delay -= dt;
    if (!m_flags.Test(2) && m_delay < 0.0f) {
        fn_800D0B64(m_model->field_0x4, boost::bind(&ItemBox::OnColliDetect, this, _1));
        boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), "col");
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback = AttackCallback(boost::bind(&ItemBox::OnAttack, this, _1));
        }
        m_flags.Set(2);
    }

    switch (m_mode) {
    case 0:
        if (m_param == 10) {
            m_state.m_item = 0;
            m_mode = 2;
        } else {
            m_mode = 1;
        }
        LoadClump();
        break;
    case 2:
        m_state.m_time -= dt;
        if (m_state.m_time <= 0.0f) {
            m_state.m_time = 0.5f;
            m_state.m_item++;
            if (m_state.m_item > 8) {
                m_state.m_item = 0;
            }
            LoadClump();
        }
        break;
    case 3:
        m_flags.Set(1);
        break;
    case 4:
        m_flags.Set(1);
        break;
    }
}

bool ItemBox::IsOutOfRange(float distanceSquared) {
    return fn_80169BE4(&m_chara->m_position, distanceSquared);
}

void ItemBox::SetPlacement(const Vec3f &position, Vec3f &rotation) {
    fn_800091D0(&m_chara->m_position, &position);
    CopyRotation(m_chara->m_rotation, rotation);
    UpdateMatrix();
}

void ItemBox::UpdateMatrix() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
    fn_800091D0(&m_state.m_position, &m_chara->m_position);
    CopyRotation(m_state.m_rotation, m_chara->m_rotation);
    m_state.m_time = 0.0f;
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void ItemBox::OnColliDetect(ColliDetectArgs &args) {
    if (m_flags.Test(0) && (m_mode == 1 || m_mode == 2)) {
        ItemBoxHitContext context;

        fn_800091D0(&context.m_position, &m_chara->m_position);
        context.m_item = m_state.m_item;
        fn_8015B004(args, GiveItem, &context);
        if (context.m_given) {
            m_mode = 3;
        }
    }
}

void ItemBox::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_flags.Test(0) && (m_mode == 1 || m_mode == 2)) {
        boost::weak_ptr<CharaColliAttack> attack = param.m_attacker;
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            if (boost::shared_ptr<CharInfo> attacker = collision->m_owner.lock()) {
                if (attacker->vf_0x1C()) {
                    ItemBoxHitContext context;

                    fn_800091D0(&context.m_position, &m_chara->m_position);
                    context.m_item = m_state.m_item;
                    GiveItem(attacker, &context);
                    m_mode = 3;
                    param.m_result = 3;
                }
            }
        }
    }
}

void ItemBox::LoadClump() {
    static const s32 s_clumps[11] = {0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x73,
                                     0x70, 0x71, 0x74, 0x72, -1};

    if (s_clumps[m_state.m_item] != -1) {
        m_state.m_clump = fn_8001277C(fn_800122D4(), 0, s_clumps[m_state.m_item]);
        m_state.m_box = fn_8001277C(fn_800122D4(), 0, 0x68);
    } else {
        m_state.m_clump = 0;
        m_state.m_box = 0;
    }
}

void ItemBox::Draw(RednerDrawArgs &args) {
    if (m_state.m_clump == 0 || m_state.m_box == 0) {
        return;
    }
    RwSphere sphere;

    sphere.radius = 10.0f;
    sphere.center.x = m_state.m_position.x;
    sphere.center.y = m_state.m_position.y;
    sphere.center.z = m_state.m_position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwFrame *frame = (RwFrame *)m_state.m_clump->object.parent;
        Vec3f angles = GetCameraAngles(fn_80009548(), 0);

        fn_80109AF8(frame, angles.z, 0);
        fn_800E9C64(frame, angles.x, 2);
        fn_800E9CEC(frame, angles.y, 2);
        fn_804875B0(frame, &sphere.center, 2);
        fn_80456BC4(m_state.m_clump);
    }
}

void ItemBox::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x1A:
        m_flags.Reset(0);
        if (m_target->IsEnabled()) {
            m_target->TurnOff();
        }
        break;
    case 0x1B:
        m_flags.Set(0);
        if (!m_target->IsEnabled()) {
            m_target->TurnOn();
        }
        break;
    case 0x14: /* VacuumCommand */
        if (!m_flags.Test(1)) {
            int item = m_param;
            VacuumCommand &vacuum = static_cast<VacuumCommand &>(command);
            if (item == 10) {
                item = m_state.m_item;
            }
            VacuumItemBox::Start(vacuum, item, m_chara->m_position, m_chara->m_rotation);
            m_mode = 4;
        }
        break;
    }
}

ItemBoxTask::ItemBoxTask(Task *parent, boost::shared_ptr<ItemBox> box) : Task(parent) {
    m_name = s_taskName;
    m_box = box;
}

ItemBoxTask::~ItemBoxTask() {}

void ItemBoxTask::Tick(float dt) {
    if (m_box.get() != 0) {
        if (m_box->IsOutOfRange(1000000.0f)) {
            m_flags |= 1;
        } else if (m_box->IsDone()) {
            m_flags |= 1;
        } else {
            m_box->Tick(dt);
        }
    }
}

SetItemBoxTask::SetItemBoxTask(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_param(0) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x17, 4, "ItemBox");
    LoadFromOwner(owner);
    m_box = boost::shared_ptr<ItemBox>(new ItemBox(m_item, 0.0f));
    PlaceBox();
}

SetItemBoxTask::~SetItemBoxTask() {}

void SetItemBoxTask::Behave(float dt) {
    if (m_box.get() != 0) {
        if (m_box->IsDone()) {
            if (m_owner != 0) {
                fn_800C9B34(m_owner);
            }
            m_flags |= 1;
        } else {
            m_box->Tick(dt);
        }
    }
}

void SetItemBoxTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 previous = m_item;

    LoadFromOwner(owner);
    PlaceBox();
    if (previous != m_item) {
        m_box.reset();
        m_box = boost::shared_ptr<ItemBox>(new ItemBox(m_item, 0.0f));
    }
}

void SetItemBoxTask::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_param, sizeof(m_param));
    m_item = m_param;
}

void SetItemBoxTask::PlaceBox() {
    if (m_box.get() != 0) {
        m_box->SetPlacement(m_chara->m_position, m_chara->m_rotation);
    }
}

} // namespace SetItem

extern "C" void fn_8010BBC0(void) {
    fn_8006B50C(0xD7, 1);
}

extern "C" void fn_8010BBBC(void) {}

namespace SetItem {

void SetItemBoxTask::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetItemBoxTask(fn_801EB304(fn_8001C4F8(), 11), owner);
}

} // namespace SetItem

/* The label of the "item" parameter: the item's name. */
extern "C" const char *fn_8010BB24(UnkLabelQuery_800CBC20 *cursor) {
    if (cursor->index == 0) {
        return ItemName(cursor->value);
    }
    return 0;
}

static GadgetParamDesc lbl_80526560[] = {
    {1, "item", 0, 0, 10, 0.0f, 0.0f, 0.0f, fn_8010BB24, 0},
    {0},
};

GadgetClassDesc lbl_805265B0 = {
    "ITEM_BOX", fn_8010BBC0, fn_8010BBBC, SetItem::SetItemBoxTask::Instantiate, 0, 0, 0x12, 1, 0, 10,
    lbl_80526560,
};

/* Drops a box holding item at position, facing rotation; the box sets its collision callbacks
 * after delay (name unknown). */
extern "C" void fn_8010B824(int item, const Vec3f &position, Vec3f &rotation,
                            float delay) {
    if (item >= 0 && item < 11) {
        boost::shared_ptr<SetItem::ItemBox> box =
            boost::shared_ptr<SetItem::ItemBox>(new SetItem::ItemBox(item, delay));
        box->SetPlacement(position, rotation);
        Task *parent = fn_801EB304(fn_8001C4F8(), 11);
        new SetItem::ItemBoxTask(parent, box);
    }
}

inline const char *ItemName(int item) {
    static const char *s_names[11] = {"Ring5",    "Ring10",        "Ring20",   "Barrier",
                                      "Heatbarrier", "Magnetbarrier", "Unbeaten", "Blank",
                                      "1UP",      "Recover",       "Roulette"};

    if (item >= 0 && item < 11) {
        return s_names[item];
    }
    return 0;
}

/* The name of item (Game/ContainerNormal.cpp and others call it by this name). */
extern "C" const char *fn_8010B7F8(int item) {
    return ItemName(item);
}

namespace SetItem {

VacuumItemBox::VacuumItemBox(VacuumCommand &command, int item)
    : VacuumObject(fn_80237EDC(&command), command), m_item(item) {
    m_model = fn_8015F9D8(0, 0xD7);
    m_model->fn_80162A68(m_item, 0);
}

void VacuumItemBox::Tick(float dt) {
    VacuumObject::Tick(dt);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_802382B0(this));
    Vec3f rotation = fn_80238248(this);
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &position);
        fn_8040B848(&matrix, rotation.y);
        fn_8040B884(&matrix, rotation.x);
        fn_8040B80C(&matrix, rotation.z);
        m_model->fn_801633DC(&matrix);
        m_model->fn_80162FB0(dt);
    }
}

void VacuumItemBox::fn_80238180() {
    boost::shared_ptr<CharInfo> character = fn_80238058(this).lock();
    if (character.get() != 0) {
        ItemBoxHitContext context;

        AssignVector(context.m_position, fn_802382B0(this));
        context.m_item = m_item;
        context.m_given = false;
        GiveItem(character, &context);
    }
}

void VacuumItemBox::Start(VacuumCommand &command, int item, const Vec3f &position,
                          const Vec3f &rotation) {
    boost::shared_ptr<VacuumObject> object =
        boost::shared_ptr<VacuumObject>(new VacuumItemBox(command, item));
    object->fn_802382E0(position);
    object->fn_80238268(rotation);
    new VacuumObjectTask(object);
}

VacuumItemBox::~VacuumItemBox() {}

} // namespace SetItem
