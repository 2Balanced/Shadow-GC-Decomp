/* MonsterControl (class name from RTTI): the MonsterControl gadget, a box trigger of the
 * "X/Y/Z_LENGTH m" parameters' size (metres, scaled by ten) at the owner's position. When a
 * character enters it, it looks up the monster's character info (character id 0x4B) and passes it
 * the trigger's mode (the "0:START 1:MISS 2:END" parameter); the monster then switches the trigger
 * off. File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define BOOST_SP_STATIC_POINTER_CAST

/* Before boost/bind.hpp: template instances come out in the order their templates are defined, and
 * retail has static_pointer_cast ahead of boost::bind. */
#include "boost/shared_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MonsterCharInfo.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The collision event a collider reports (name from the bound callback's RTTI). Only its first
 * word, which the hit test reads, is mapped (name inferred). */
struct ColliDetectArgs {
    /* 0x0 */ void *m_info;
};

/* The event type of a handle's callbacks (name from the RTTI of CatchStick's callback binder). */
struct GadgetColliDetectArgs : public ColliDetectArgs {};

/* The function a handle calls back with a collision event (name inferred). */
typedef boost::function1<void, GadgetColliDetectArgs &> GadgetColliFunction;

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * FireRing). */
class GadgetCharCollision {
  public:
    /* Sets flag bit 1, which stops the handle's collisions (name and meaning inferred). */
    void SetFlag2() {
        m_flags |= 2;
    }
    /* Clears flag bit 1 (name and meaning inferred). */
    void ClearFlag2() {
        m_flags &= ~2;
    }

    /* 0x00 */ GadgetColliFunction m_onContact;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
};

/* The collider manager (behaviour inferred). */
class UnkColliderManager_80021B08;
/* The character manager fn_800DDF20 returns (behaviour inferred). */
class UnkCharManager_800DDF20;


extern "C" {
/* Returns the collider manager, and creates a box collider at position (behaviour and argument
 * meanings inferred, as in ContainerBehavior). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase>
fn_800C7D04(UnkColliderManager_80021B08 *manager, const Vec3f &position,
            const Vec3f &size, const Vec3f &rotation, int arg4, int arg5, int arg6,
            int arg7, const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11);
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision>
fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Assigns callback to one of a handle's callbacks (behaviour inferred). */
void fn_800F5980(GadgetColliFunction *target, const GadgetColliFunction &callback);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* True when the collision event comes from a player character (behaviour inferred). */
bool fn_8015D2EC(void *info);
/* The character manager, and the character registered under id (behaviour inferred, as in
 * CityBombSmall). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
boost::weak_ptr<CharInfo> *fn_801DCB14(UnkCharManager_800DDF20 *manager, int id);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
/* A vector constant read by the collision setup (meaning unknown). */
extern "C" Vec3f lbl_805E07B8;

/* The four parameters the owner supplies (names from the parameter table). */
struct MonsterControlParams {
    /* 0x0 */ int mode; /* 0 start, 1 miss, 2 end */
    /* 0x4 */ float sizeX; /* metres in the table, scaled by ten when loaded */
    /* 0x8 */ float sizeY;
    /* 0xC */ float sizeZ;
};
SIZE_ASSERT(MonsterControlParams, 0x10);

class MonsterControl : public Gadget {
  public:
    MonsterControl(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~MonsterControl();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the trigger under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's parameters, position and rotation (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Moves and resizes the collider to the trigger (name inferred). */
    void UpdateCollider();
    /* The handle's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Stops the handle's collisions; the monster calls it (name inferred). */
    void Stop();

    static const char *s_taskName;

    /* 0x30 */ Vec3f m_position;
    /* 0x3C */ Vec3f m_rotation;
    /* 0x48 */ MonsterControlParams m_params;
    /* 0x58 */ boost::shared_ptr<GadgetCharCollision> m_handle;
};
SIZE_ASSERT(MonsterControl, 0x60);

const char *MonsterControl::s_taskName = "MonsterControl";

MonsterControl::MonsterControl(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_rotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    LoadFromOwner(owner);
    Vec3f size(m_params.sizeX, m_params.sizeY, m_params.sizeZ);
    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7D04(fn_80021B08(), m_position, size, m_rotation, 0, 0x100, 10, 0, lbl_805EE698,
                    lbl_805E07A0, lbl_805E07B8, 0);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(&m_handle->m_onContact, boost::bind(&MonsterControl::OnColliDetect, this, _1));
    m_handle->ClearFlag2();
}

MonsterControl::~MonsterControl() {}

extern "C" void fn_8013A594(void) {}

extern "C" void fn_8013A590(void) {}

void MonsterControl::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new MonsterControl(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void MonsterControl::Behave(float dt) {
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
}

void MonsterControl::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateCollider();
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void MonsterControl::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.sizeX = 10.0f * m_params.sizeX;
    m_params.sizeY = 10.0f * m_params.sizeY;
    m_params.sizeZ = 10.0f * m_params.sizeZ;
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_rotation);
}

void MonsterControl::UpdateCollider() {
    if (m_handle.get() != 0) {
        m_handle->m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_position));
        m_handle->m_collider->SetRotation(&m_rotation);
        m_handle->m_collider->SetSize(m_params.sizeX, m_params.sizeY, m_params.sizeZ, -2);
    }
}

void MonsterControl::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015D2EC(args.m_info)) {
        boost::weak_ptr<CharInfo> *monster = fn_801DCB14(fn_800DDF20(), 0x4B);
        if (monster != 0) {
            boost::shared_ptr<MonsterCharInfo> info =
                boost::static_pointer_cast<MonsterCharInfo>(monster->lock());
            if (info.get() != 0) {
                info->Issue(m_params.mode, this);
            }
        }
    }
}

void MonsterControl::Stop() {
    if (m_handle.get() != 0) {
        m_handle->SetFlag2();
    }
}

static GadgetParamDesc lbl_80528ED4[] = {
    {1, "0:START 1:MISS 2:END", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "X_LENGTH m", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Y_LENGTH m", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Z_LENGTH m", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80528F9C = {
    "MonsterControl", fn_8013A594, fn_8013A590, MonsterControl::Instantiate, 0, 0, 0xBC8, 1, 0, 10,
    lbl_80528ED4,
};
