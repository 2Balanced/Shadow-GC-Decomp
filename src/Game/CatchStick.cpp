/* CatchStick (class name from RTTI): the CATCHSTICK gadget, a bar a player character grabs. It keeps
 * a line of the "length" parameter along its local Z axis (a Sonicteam::System::Geometry::Line) and
 * a capsule collider around it. A player that touches the collider while close enough to the line
 * gets a TouchLineCommand for it. Its command unit ignores every command. File name inferred from
 * the class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). The unit's code reaches its .data
 * through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/GeometryLine.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls. GC/2.7 emits a local copy of the inline template instead, and the linker
 * does not resolve a declared-only specialization of it, so an overload declared here stands in
 * for that shared copy. */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Sonicteam::System::Geometry::Line;

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * ElecCircuitColli). */
class GadgetCharCollision {
  public:
    /* 0x00 */ boost::function1<void, GadgetColliDetectArgs &> m_onContact;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ CharaColliBase *m_collider;
};

/* A part of a player character fn_80078CEC returns (class name unknown); its position lies 0xC
 * bytes in (the two parts read are probably the hands; inferred). */
struct UnkPlayerPart_80078CEC {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ Vec3f m_position;
};

extern "C" {
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a capsule collider for charInfo (behaviour inferred; the meanings of the other arguments
 * are unknown). */
boost::shared_ptr<CharaColliBase> fn_800C78D4(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, float halfLength,
                                              float radius, const Vec3f *offset,
                                              const Vec3f *rotation, int arg9);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Re-registers the handle's collider, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AE44(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Returns part index of player (behaviour inferred). */
UnkPlayerPart_80078CEC *fn_80078CEC(Player::PlayerBase *player, int index);
/* The distance of point from line, and between two lines (behaviour inferred; the integer
 * arguments are unknown). */
float fn_8003C660(const Vec3f *point, const Line *line, int arg2);
float fn_8003C4C4(const void *segment, const Line *line, int arg2, int arg3);
}

/* Sets the handle's contact callback and returns the handle (behaviour from its body). An inline
 * function in the original: it is emitted as a weak copy among this unit's template instances, and
 * the other users call that copy. */
extern "C" inline GadgetCharCollision *
fn_800F5980(GadgetCharCollision *handle,
            const boost::function1<void, GadgetColliDetectArgs &> &callback) {
    handle->m_onContact = callback;
    return handle;
}

class CatchStick : public Gadget {
  public:
    CatchStick(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CatchStick();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The collider's callback (name inferred). */
    void OnColliDetect(GadgetColliDetectArgs &args);
    /* The command unit's callback; it ignores every command (name inferred). */
    void OnCommand(CharCommand &command);
    /* Places the line at the character info's position and rotation (name inferred). */
    void UpdateLine();
    /* Copies the owner's position, rotation and length (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ float m_length;
    /* 0x3C */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x44 */ boost::shared_ptr<Line> m_line;
    /* 0x4C */ u32 pad_0x4C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(CatchStick, 0x50);

/* What the collision callback passes to the per-character hit function (name inferred). */
struct CatchStickHitContext {
    CatchStickHitContext() {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ boost::weak_ptr<Line> m_line;
};
SIZE_ASSERT(CatchStickHitContext, 0x10);

/* Hands the stick's line to a player whose two parts are close enough to it (name inferred).
 * Retail emits it after the constructor and its template instances, so it is defined before the
 * constructor. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    CatchStickHitContext *context = static_cast<CatchStickHitContext *>(user);

    if (hit.get() != 0 && context != 0) {
        Player::PlayerBase *player = dynamic_cast<Player::PlayerBase *>(hit.get());
        if (player != 0) {
            boost::shared_ptr<Line> line = context->m_line.lock();
            if (line.get() != 0) {
                u8 segment[0x18];
                Vec3f start;
                Vec3f end;
                Vec3f offset;
                Vec3f difference;
                float distance;

                UnkPlayerPart_80078CEC *part = fn_80078CEC(player, 0);
                ConstructVector(&start, part->m_position);
                part = fn_80078CEC(player, 1);
                ConstructVector(&end, part->m_position);
                fn_8040CD74(&difference, &start, &end);
                if (fn_8040CF50(fn_800091EC(&offset, &difference)) < 0.1f) {
                    distance = fn_8003C660(&start, line.get(), 0);
                } else {
                    fn_80020A20(segment, &start, &end);
                    distance = fn_8003C4C4(segment, line.get(), 0, 0);
                }
                if (distance > 144.0f) {
                    return;
                }
            }
            hit->PushCommand(new Player::TouchLineCommand(context->m_chara, context->m_line, 4));
        }
    }
}

/* Defined after HitCharacter: retail places the Player RTTI records HitCharacter's dynamic_cast
 * emits (and their names) before s_taskName and its string. */
const char *CatchStick::s_taskName = "CatchStick";

CatchStick::CatchStick(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x38, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&CatchStick::OnCommand, this, _1)));
    LoadFromOwner(m_owner);
    boost::shared_ptr<CharaColliBase> collider =
        fn_800C78D4(fn_80021B08(), m_chara, 0x40000, 10, 0, 0.5f * m_length, 2.0f, &lbl_805E07A0,
                    &lbl_805E07B8, 0);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(m_handle.get(), boost::bind(&CatchStick::OnColliDetect, this, _1));
    m_line = boost::shared_ptr<Line>(new Line());
    UpdateLine();
}

CatchStick::~CatchStick() {}

void CatchStick::Behave(float dt) {
    fn_8015BE9C(m_handle.get(), dt);
}

void CatchStick::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateLine();
    if (m_handle.get() != 0) {
        m_handle->m_collider->SetSize(2.0f, 0.0f, 0.5f * m_length, -2);
        fn_8015BE64(m_handle.get());
        fn_802E0B58(m_line.get(), &lbl_805F06D8);
    }
}

void CatchStick::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_length, sizeof(m_length));
}

void CatchStick::UpdateLine() {
    Mtx34f matrix;
    Vec3f start(0.0f, 0.0f, 1.0f);
    Vec3f end(0.0f, 0.0f, -1.0f);

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_8040B720(&matrix, 0.0f, 0.0f, 0.5f * m_length);
    fn_800091D0(&m_line->m_start, &(const Vec3f &)fn_8040B67C(&matrix, &start));
    fn_800091D0(&m_line->m_end, &(const Vec3f &)fn_8040B67C(&matrix, &end));
}

void CatchStick::OnCommand(CharCommand &command) {}

void CatchStick::OnColliDetect(GadgetColliDetectArgs &args) {
    CatchStickHitContext context;

    context.m_chara = m_chara;
    context.m_line = m_line;
    fn_8015AE44(args, HitCharacter, &context);
}

extern "C" void fn_800F4B5C(void) {}

extern "C" void fn_800F4B58(void) {}

void CatchStick::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new CatchStick(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80525228[] = {
    {4, "length", 0, 0, 0, 10.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80525278 = {
    "CATCHSTICK", fn_800F4B5C, fn_800F4B58, CatchStick::Instantiate, 0, 0, 0x5A, 1, 0, 10, lbl_80525228,
};
