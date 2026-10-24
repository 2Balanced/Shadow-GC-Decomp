/* ElecCircuitEntrance (class name from RTTI): the ELEC_ENTRANCE gadget, the entrance to the
 * electric circuit. It creates a cylinder collider at the owner's position and binds the handle's
 * two collision callbacks. A player touching it is sent an ElectricCircuitCommand towards a point in
 * front of the entrance, and the entrance's circuit is switched on; a player that is riding is
 * remembered instead. File name inferred from the class. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * As in ElecFireWall.cpp, the constructor addresses .data through its start, so the unit's .data is
 * linked whole, led by the RTTI base lists of the two first headers (in this order). */
#include <cmath>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/PathCtrl.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The collision events of a handle (class names from RTTI: the handle passes a
 * GadgetColliDetectArgs, whose base the gadget's callbacks take). Their members are only passed
 * through here. */
struct ColliDetectArgs {};
struct GadgetColliDetectArgs : public ColliDetectArgs {};

/* The function a handle calls back (name inferred). */
typedef boost::function1<void, GadgetColliDetectArgs &> ColliDetectFunction;

/* A callback member of a handle: a ColliDetectFunction set through fn_800F5980 (layout beyond the
 * function unknown). */
struct UnkColliCallback_800F5980 {
    /* 0x0 */ u8 field_0x0[0xC];
};

/* The handle fn_8015B8C4 wraps a collider in (see DarkSpinEntrance.cpp; only the members used here
 * are declared). */
class GadgetCharCollision {
  public:
    /* Clears flag bit 1 (name and meaning inferred). */
    void ClearFlag2() {
        m_flags &= ~2;
    }

    /* 0x00 */ UnkColliCallback_800F5980 m_onDetect; /* names inferred */
    /* 0x0C */ u8 field_0xC[0xC];
    /* 0x18 */ UnkColliCallback_800F5980 m_onRelease;
    /* 0x24 */ u8 field_0x24[0x8];
    /* 0x2C */ u32 m_flags;
};

/* The manager returned by fn_80021B08, which creates colliders (behaviour inferred). */
class UnkColliderManager_80021B08;

class UnkPathManager_8001B494;

/* UnkCircuitTarget_800ABCE8 (Game/UnkCircuitTarget_800ABCE8.h): what an ElectricCircuitCommand
 * carries. */

namespace Player {

/* ElectricCircuitCommand (class name from RTTI; Player::PlayerCommand its base): sends a player
 * into the electric circuit. Its vtable is defined with the player code. */
class ElectricCircuitCommand : public PlayerCommand {
  public:
    ElectricCircuitCommand(const UnkCircuitTarget_800ABCE8 &target, int arg1, u8 arg2)
        : PlayerCommand(0x131, arg1), m_target(target), field_0x1C(arg2) {}
    virtual ~ElectricCircuitCommand();

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_target;
    /* 0x1C */ u8 field_0x1C;
};

} // namespace Player

class UnkSoundManager_80053164;

extern "C" {
/* Returns the collider manager, constructing it on first use (behaviour inferred). */
UnkColliderManager_80021B08 *fn_80021B08(void);
/* Creates a collider for charInfo (see DarkSpinEntrance.cpp). */
boost::shared_ptr<CharaColliBase>
fn_800C7A84(UnkColliderManager_80021B08 *manager, const boost::weak_ptr<CharInfo> &charInfo,
            int flags, int arg3, u16 arg4, const Vec3f *offset,
            const Vec3f *rotation, float height, float radius, int arg9);
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *target);
void fn_8015BE9C(GadgetCharCollision *target, float dt);
/* Sets a callback of a handle (behaviour inferred). */
void fn_800F5980(UnkColliCallback_800F5980 *callback, const ColliDetectFunction &function);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* True while the character rides something (behaviour inferred). */
bool fn_800B78B0(const boost::shared_ptr<CharInfo> &character);
/* The path manager, and filling the command's path from the character and a direction
 * (behaviour inferred; the meanings of the other arguments are unknown). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001CDA0(UnkPathManager_8001B494 *manager, PathCtrl *path,
                 GadgetCharInfo *charInfo, const Vec3f *direction, int *out, int arg5,
                 int arg6, float arg7);
/* vector += value (see fn_8040CF90.cpp). */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* Queues a debug cylinder (see DarkSpinEntrance.cpp). */
void fn_802E0D6C(const Vec3f *center, const Vec3f *rotation, int axis,
                 float radius, float height);
}

/* vector += value through fn_8040CFF8 (name inferred). */
inline Vec3f &operator+=(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
    return vector;
}

class ElecCircuitEntrance : public Gadget {
  public:
    ElecCircuitEntrance(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCircuitEntrance();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the entrance under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and circuit id into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Queues the handle's shape and a debug cylinder in front of the entrance (name inferred). */
    void DrawDebugShape();
    /* The handle's callbacks (names inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    void OnColliRelease(ColliDetectArgs &args);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ u8 field_0x38; /* copied from the owner; the circuit id it switches (inferred) */
    /* 0x3C */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x44 */ boost::weak_ptr<CharInfo> m_rider; /* the riding character it remembers (inferred) */
    /* 0x4C */ u32 field_0x4C;
};
SIZE_ASSERT(ElecCircuitEntrance, 0x50);

/* What the collision callback passes to the per-character function (name inferred). */
struct ElecCircuitEntranceHitContext {
    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ ElecCircuitEntrance *m_entrance;
};

const char *ElecCircuitEntrance::s_taskName = "ElecCircuitEntrance";

/* The collider's rotation (radians), a quarter turn about x, built at startup. */
static Vec3f lbl_80583B78(0.5f * lbl_805FAD38, 0.0f, 0.0f);

/* The kind CharInfo keeps at 0x50; 1 for a player (meaning inferred; CharInfo.h does not map it). */
inline s32 GetCharKind(const CharInfo *info) {
    return *(const s32 *)((const u8 *)info + 0x50);
}

/* Stores sin(angle) and cos(angle) scaled by radius through x and z (name inferred). Several units
 * call the copy emitted here. */
inline void GetCirclePoint(float *x, float *z, float angle, float radius) {
    *x = radius * std::sin(angle);
    *z = radius * std::cos(angle);
}

/* Sends hit into the circuit unless it is the remembered rider (name inferred). Defined before the
 * constructor: the original emits it after the constructor's template instances. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    ElecCircuitEntranceHitContext *context = static_cast<ElecCircuitEntranceHitContext *>(user);

    if (hit.get() != 0 && context != 0 && GetCharKind(hit.get()) == 1) {
        if (!context->m_entrance->m_rider.expired()) {
            boost::shared_ptr<CharInfo> rider(context->m_entrance->m_rider);
            if (rider == hit) {
                return;
            }
        }
        if (fn_800B78B0(hit)) {
            context->m_entrance->m_rider = hit;
            return;
        }

        PathCtrl path;
        Vec3f direction;
        int arg;

        fn_800091EC(&direction, &lbl_805E07A0);
        GetCirclePoint(&direction.x, &direction.z,
                       context->m_entrance->m_chara->m_rotation.y, -1.0f);
        if (fn_8001CDA0(fn_8001B494(), &path, context->m_entrance->m_chara.get(), &direction, &arg,
                        0, 0, 0.0f)) {
            UnkCircuitTarget_800ABCE8 target(boost::weak_ptr<CharInfo>(hit), &path, arg);
            hit->PushCommand(new Player::ElectricCircuitCommand(target, 4, 0));
            if (fn_8016904C(fn_800CB69C(), context->m_entrance->field_0x38) != 1) {
                fn_801690D0(fn_800CB69C(), context->m_entrance->field_0x38, 1);
            }
        }
    }
}

ElecCircuitEntrance::ElecCircuitEntrance(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x59, 4, "ElecCircuitEntrance");
    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7A84(fn_80021B08(), m_chara, 0x40000, 10, 0, &lbl_805E07A0, &lbl_80583B78, 7.5f,
                    20.0f, 0);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(&m_handle->m_onDetect,
                boost::bind(&ElecCircuitEntrance::OnColliDetect, this, _1));
    fn_800F5980(&m_handle->m_onRelease,
                boost::bind(&ElecCircuitEntrance::OnColliRelease, this, _1));
    m_handle->ClearFlag2();
    ReadPlacement(m_owner);
}

ElecCircuitEntrance::~ElecCircuitEntrance() {}

void ElecCircuitEntrance::Behave(float dt) {
    fn_8015BE9C(m_handle.px, dt);
}

void ElecCircuitEntrance::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void ElecCircuitEntrance::fn_800CBC1C() {
    DrawDebugShape();
}

void ElecCircuitEntrance::DrawDebugShape() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());

        Vec3f center;
        fn_800091EC(&center, &lbl_805E07A0);
        GetCirclePoint(&center.x, &center.z, m_chara->m_rotation.y, -10.0f);
        center += m_chara->m_position;
        float angle = m_chara->m_rotation.y;
        Vec3f rotation(0.0f, angle, 0.0f);
        fn_802E0D6C(&center, &rotation, 0, 10.0f, 3.75f);
    }
}

void ElecCircuitEntrance::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &field_0x38);
}

void ElecCircuitEntrance::OnColliDetect(ColliDetectArgs &args) {
    ElecCircuitEntranceHitContext context;

    context.m_chara = m_chara;
    context.m_entrance = this;
    fn_8015AA84(args, HitCharacter, &context);
}

void ElecCircuitEntrance::OnColliRelease(ColliDetectArgs &args) {
    if (!m_rider.expired()) {
        boost::shared_ptr<CharInfo> rider(m_rider);
        if (rider.get() != 0) {
            m_rider.reset();
        }
    }
}

extern "C" void fn_801F7D7C(void) {}

extern "C" void fn_801F7D78(void) {}

void ElecCircuitEntrance::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCircuitEntrance(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_80542370 = {
    "ELEC_ENTRANCE", fn_801F7D7C, fn_801F7D78, ElecCircuitEntrance::Instantiate, 0, 0, 0x7E0, 1, 0, 10, 0,
};
