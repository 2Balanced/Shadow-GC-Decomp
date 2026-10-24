/* UpDownReel::UpDownReel (class name from RTTI): the UD_REEL gadget, a reel on a rope built from
 * three parts of model file 0xE5 (the base, the rope, scaled to the rope's length, and the hanging
 * reel). A character that rides it (RideCommand) is hung on the reel through TouchHangCommands
 * while the rope winds from "length" to "pos", with a winding sound; the character's input
 * (AccelerateInputCommand) turns the reel. When the character leaves (LeaveCommand) it is launched
 * along the reel's facing, tilted by "angle", at speed "power". A collider on the reel catches
 * characters that touch it and hangs them as well. File name inferred from the namespace. Method
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AngleUtil.h"
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
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/GeometryLine.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Sonicteam::System::Geometry::Line;

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * ElecCircuitColli). */
class GadgetCharCollision {
  public:
    /* Sets flag bit 0 and the float at 0x30 to 1 (name and meaning inferred). */
    void LetGo() {
        m_flags |= 1;
        field_0x30 = 1.0f;
    }

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
    /* 0x30 */ float field_0x30;
};

/* RideCommand (command type 0xB) is declared in Game/CharCommand.h. */

/* AccelerateInputCommand (command type 1) is declared in Game/CharCommand.h. */

/* A 32-bit flag word whose constructor clears it (fn_80014E04): fn_80014E10 clears it, fn_80014E1C
 * sets bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour inferred from their
 * bodies; name inferred). */
extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
}

struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

extern "C" {
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a collider for charInfo (behaviour inferred; the meanings of the arguments are
 * unknown). */
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, CharInfo *charInfo, int flags, int arg3,
                                              int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AE44(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Starts sound soundId for owner (behaviour inferred). */
UnkSoundHandle_801D3DC8 fn_801D3EF0(int soundId, void *owner);
/* The pitch and yaw of direction (behaviour inferred from its body). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
/* matrix translated by (x, y, z) (behaviour inferred). */
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
/* a + b, and vector * scale (behaviour from their bodies). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* vector = (0, 0, 0) through a zero constant (behaviour from its body). */
void fn_8040E970(Vec3f *vector);
/* Passed to the second debug line's draw call (meaning unknown). */
extern u32 lbl_805F1370;
}

/* An empty character pointer (meaning inferred; several gadgets pass it to the collider setup). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* The step the reel turns back by each frame (4 degrees), and pi (names inferred). */
static float s_returnStep = 4.0f * (lbl_805FAD38 / 180.0f);
static const float s_halfTurn = 180.0f * (lbl_805FAD38 / 180.0f);

namespace UpDownReel {

/* The four parameters the owner supplies (names from the parameter table). */
struct UpDownReelParams {
    UpDownReelParams() : length(0.0f), pos(0.0f), angle(0.0f), power(0.0f) {}

    /* 0x0 */ float length;
    /* 0x4 */ float pos;
    /* 0x8 */ float angle; /* degrees */
    /* 0xC */ float power;
};
SIZE_ASSERT(UpDownReelParams, 0x10);

class UpDownReel : public Gadget {
  public:
    UpDownReel(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~UpDownReel();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the reel under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Launches the rider (name inferred). */
    void Launch();
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Places the three model parts, the collider and the hang line (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters, and resets the state (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* The collider's callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ GadgetModelInfo *m_models[3]; /* the base, the reel and the rope (inferred) */
    /* 0x44 */ UpDownReelParams m_params;
    /* 0x54 */ s32 m_state; /* 0 idle, 1 winding up, 2 winding down, 3 wound (inferred) */
    /* 0x58 */ float m_length; /* the rope's current length (inferred) */
    /* 0x5C */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x64 */ boost::shared_ptr<Line> m_line; /* the hang line under the reel (inferred) */
    /* 0x6C */ Vec3f m_hangPosition;    /* the middle of the hang line (inferred) */
    /* 0x78 */ Vec3f m_rotation;        /* the reel's turn (inferred) */
    /* 0x84 */ Vec3f m_hangRotation;    /* (inferred) */
    /* 0x90 */ boost::weak_ptr<CharInfo> m_rider;
    /* 0x98 */ UnkFlags_80014E04 m_flags; /* bit 0 ridden, 1 left, 2 launch cancelled, 3 updated
                                           * (inferred) */
    /* 0x9C */ UnkSoundHandle_801D3DC8 m_sound;
};

/* What the collider's callback passes to the per-character hit function (name inferred). */
struct UpDownReelHitContext {
    UpDownReelHitContext() : m_hit(false) {}

    /* 0x00 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ boost::weak_ptr<Line> m_line;
    /* 0x10 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x18 */ bool m_hit;
};
SIZE_ASSERT(UpDownReelHitContext, 0x1C);

} // namespace UpDownReel

SIZE_ASSERT(UpDownReel::UpDownReel, 0xB8);

namespace UpDownReel {

const char *UpDownReel::s_taskName = "UpDownReel";

/* Records the character the collider touched (name inferred). Retail emits it after the
 * constructor and its template instances, so it is defined before the constructor. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    UpDownReelHitContext *context = static_cast<UpDownReelHitContext *>(user);

    if (hit.get() != 0 && context != 0) {
        context->m_character = hit;
    }
    context->m_hit = true;
}

UpDownReel::UpDownReel(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_rotation(0.0f, 0.0f, 0.0f), m_hangRotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x18, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&UpDownReel::OnCommand, this, _1)));

    m_models[0] = new GadgetModelInfo(0, 0xE5);
    m_models[0]->fn_80162A68(0, 0);
    m_models[2] = new GadgetModelInfo(0, 0xE5);
    m_models[2]->fn_80162A68(2, 0);
    m_models[1] = new GadgetModelInfo(0, 0xE5);
    m_models[1]->fn_80162A68(1, 0);
    m_state = 0;

    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7EA8(fn_80021B08(), m_chara.get(), 0x40000, 10, 0, lbl_805EE698, &lbl_805E07A0, 12.0f);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(m_handle.get(), boost::bind(&UpDownReel::OnColliDetect, this, _1));
    m_line = boost::shared_ptr<Line>(new Line());
    LoadFromOwner(m_owner);
    RebuildTransform();
}

UpDownReel::~UpDownReel() {
    m_sound.fn_801D455C();
    for (int i = 0; i < 3; i++) {
        if (m_models[i] != 0) {
            delete m_models[i];
            m_models[i] = 0;
        }
    }
}

void UpDownReel::Behave(float dt) {
    fn_8006127C(&m_flags.m_bits, 3);

    switch (m_state) {
    case 0:
        if (m_params.length - m_params.pos > 0.0f) {
            m_length += 0.18f;
            if (m_length >= m_params.length) {
                m_length = m_params.length;
            }
        } else if (m_params.length - m_params.pos < 0.0f) {
            m_length -= 0.18f;
            if (m_length <= m_params.length) {
                m_length = m_params.length;
            }
        }
        break;
    case 1:
        m_length -= 0.18f;
        if (m_length <= m_params.pos) {
            m_length = m_params.pos;
            m_state = 3;
            m_sound.fn_801D455C();
        }
        break;
    case 2:
        m_length += 0.18f;
        if (m_length > m_params.pos) {
            m_length = m_params.pos;
            m_state = 3;
            m_sound.fn_801D455C();
        }
        break;
    case 3:
        /* Wound. Listing the case keeps the original's extra branch. */
        break;
    }

    RebuildTransform();
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
    for (int i = 0; i < 3; i++) {
        if (m_models[i] != 0) {
            m_models[i]->fn_80162FB0(dt);
        }
    }

    if (fn_80014E34(&m_flags.m_bits, 0)) {
        if (fn_80014E34(&m_flags.m_bits, 1)) {
            if (!fn_80014E34(&m_flags.m_bits, 2)) {
                Launch();
            }
            fn_8006127C(&m_flags.m_bits, 0);
            fn_8006127C(&m_flags.m_bits, 1);
            m_rider.reset();
            if (m_handle.get() != 0) {
                m_handle->LetGo();
            }
        } else if (boost::shared_ptr<CharInfo> rider = m_rider.lock()) {
            m_hangRotation = m_rotation;
            m_hangRotation.y += s_halfTurn + m_chara->m_rotation.y;
            rider->PushCommand(new Player::TouchHangCommand(m_chara, &m_hangPosition, &m_hangRotation, 3, 4));
        }
    } else {
        m_rotation.y = TurnAngleToward(0.0f, m_rotation.y, s_returnStep);
    }
}

void UpDownReel::OnColliDetect(ColliDetectArgs &args) {
    if (fn_80014E34(&m_flags.m_bits, 0)) {
        return;
    }
    if (fn_80014E34(&m_flags.m_bits, 3)) {
        return;
    }

    UpDownReelHitContext context;
    context.m_chara = m_chara;
    context.m_line = m_line;
    fn_8015AE44(args, HitCharacter, &context);
    if (context.m_hit) {
        if (boost::shared_ptr<CharInfo> character = context.m_character.lock()) {
            character->PushCommand(
                new Player::TouchHangCommand(m_chara, &m_hangPosition, &m_hangRotation, 3, 4));
        }
        fn_80014E1C(&m_flags.m_bits, 3);
        fn_8006127C(&m_flags.m_bits, 1);
        m_rider.reset();
    }
}

void UpDownReel::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    Mtx34f matrix;
    Mtx34f launchMatrix;
    u8 segmentX[0x18];
    u8 segmentY[0x18];
    u8 segmentZ[0x18];
    u8 segmentLaunch[0x18];

    LoadFromOwner(owner);
    RebuildTransform();
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B52C(&matrix, 0.0f, 10.0f * -m_params.pos, 0.0f);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    Vec3f center(0.0f, 0.0f, 0.0f);
    AssignVector(center, fn_8040B67C(&matrix, &center));
    Vec3f start;
    Vec3f end;

    fn_800091D0(&start, fn_800091D0(&end, &center));
    start.x -= 10.0f;
    end.x += 10.0f;
    fn_802E0B58(fn_80020A20(segmentX, &start, &end), &lbl_805F06D8);
    fn_800091D0(&start, fn_800091D0(&end, &center));
    start.y -= 10.0f;
    end.y += 10.0f;
    fn_802E0B58(fn_80020A20(segmentY, &start, &end), &lbl_805F06D8);
    fn_800091D0(&start, fn_800091D0(&end, &center));
    start.z -= 10.0f;
    end.z += 10.0f;
    fn_802E0B58(fn_80020A20(segmentZ, &start, &end), &lbl_805F06D8);

    fn_8040C148(&launchMatrix, &lbl_805E0720);
    fn_8040B59C(&launchMatrix, &m_chara->m_position);
    fn_8040B52C(&launchMatrix, 0.0f, 10.0f * -m_length, 0.0f);
    fn_8040B848(&launchMatrix, m_chara->m_rotation.y);
    fn_8040B884(&launchMatrix, m_params.angle * (lbl_805FAD38 / 180.0f));
    Vec3f from(0.0f, -14.0f, 0.0f);
    Vec3f to(0.0f, -14.0f, 50.0f);
    AssignVector(from, fn_8040B67C(&launchMatrix, &from));
    AssignVector(to, fn_8040B67C(&launchMatrix, &to));
    fn_802E0B58(fn_80020A20(segmentLaunch, &from, &to), &lbl_805F1370);
}

void UpDownReel::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_length = m_params.length;
    m_state = 0;
    fn_80014E10(&m_flags.m_bits);
    fn_8040E970(&m_rotation);
}

void UpDownReel::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    if (m_models[0] != 0) {
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_models[0]->fn_801633DC(&matrix);
    }
    if (m_models[2] != 0) {
        fn_8040B720(&matrix, 1.0f, m_length, 1.0f);
        m_models[2]->fn_801633DC(&matrix);
    }
    if (m_models[1] != 0) {
        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B52C(&matrix, 0.0f, 10.0f * -m_length, 0.0f);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B848(&matrix, m_rotation.y);
        m_models[1]->fn_801633DC(&matrix);
        if (m_handle.get() != 0) {
            Vec3f position(0.0f, -8.0f, 0.0f);
            AssignVector(position, fn_8040B67C(&matrix, &position));
            /* The collider's slot 0x3C, given the collider's position here. */
            m_handle->m_collider->SetCharInfo((CharInfo *)&position);
        }

        Vec3f right(10.0f, -14.0f, 0.0f);
        Vec3f left(-10.0f, -14.0f, 0.0f);
        fn_800091D0(&m_line->m_start, &(const Vec3f &)fn_8040B67C(&matrix, &right));
        fn_800091D0(&m_line->m_end, &(const Vec3f &)fn_8040B67C(&matrix, &left));
        fn_800091D0(&m_hangPosition,
                    &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&m_line->m_start, &m_line->m_end), 0.5f));
    }
}

void UpDownReel::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB: /* RideCommand */
        if (m_state == 0) {
            if (m_params.length - m_params.pos > 0.0f) {
                m_state = 1;
                m_sound = fn_801D3EF0(0x1000, this);
            } else if (m_params.length - m_params.pos < 0.0f) {
                m_state = 2;
                m_sound = fn_801D3EF0(0x1000, this);
            }
            fn_80014E1C(&m_flags.m_bits, 0);
            fn_8006127C(&m_flags.m_bits, 1);
            fn_8006127C(&m_flags.m_bits, 2);
            m_rider = static_cast<RideCommand &>(command).m_partner;
        }
        break;
    case 0xC: /* LeaveCommand */
        fn_80014E1C(&m_flags.m_bits, 1);
        m_state = 0;
        m_sound.fn_801D455C();
        break;
    case 0xF:
        fn_80014E1C(&m_flags.m_bits, 2);
        break;
    case 1: { /* AccelerateInputCommand */
        static float s_turnStep = 2.0f * (lbl_805FAD38 / 180.0f);
        AccelerateInputCommand *input = static_cast<AccelerateInputCommand *>(&command);
        if (input != 0) {
            float yaw;
            float pitch;
            fn_8003B9E8(&input->m_direction, &pitch, &yaw);
            yaw -= m_chara->m_rotation.y;
            m_rotation.y = TurnAngleToward(yaw, m_rotation.y, s_turnStep);
        }
        break;
    }
    }
}

void UpDownReel::Launch() {
    if (boost::shared_ptr<CharInfo> rider = m_rider.lock()) {
        rider->PushCommand(new ClrVelocityCommand(4));

        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_params.angle * (lbl_805FAD38 / 180.0f));
        Vec3f velocity(0.0f, 0.0f, 1.0f);
        AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
        fn_8040CE38(&velocity, m_params.power);
        rider->PushCommand(new SetVelocityCommand(velocity, 4));
        rider->PushCommand(new Player::LaunchCommand(0, 4));
    }
}

} // namespace UpDownReel

extern "C" void fn_801273F8(void) {
    fn_8006B50C(0xE5, 1);
}

extern "C" void fn_801273F4(void) {}

void UpDownReel::UpDownReel::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new UpDownReel(parent, owner);
}

static GadgetParamDesc lbl_80527EF8[] = {
    {4, "length", 0, 0, 0, 1.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {4, "pos", 0, 0, 0, 1.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {4, "angle", 0, 0, 0, 0.0f, -90.0f, 90.0f, fn_800CBC20, 0},
    {4, "power", 0, 0, 0, 400.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80527FC0 = {
    "UD_REEL", fn_801273F8, fn_801273F4, UpDownReel::UpDownReel::Instantiate, 0, 0, 8, 1, 0, 10,
    lbl_80527EF8,
};
