/* Fan::Fan (class name from RTTI): the FAN and FAN STAGE gadgets, a fan that blows characters
 * along its axis. Its "type" picks the direction (0: upward, 1: sideways) and its "form" the shape
 * of its collider (0: a cylinder of radius X, 1: a box of X, Y, Z); "power" scales the push, an
 * "Id" switch turns it on and off, and "sec_active"/"sec_negative" make it run on a timer. While a
 * character is inside, an upward fan keeps lifting it (with a little bob), and a sideways fan holds
 * it in a stream; the character is released once it leaves. FAN STAGE differs only in loading its
 * model from the stage's files. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Player.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* MSL's std::cos(float), which the game calls out of line (0x8000F3B0). It is declared here rather
 * than through <cmath>, whose sqrtf adds two double constants to .sdata2 that this unit lacks. */
namespace std {
float cos(float x);
}

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * CatchStick). */
class GadgetCharCollision {
  public:
    /* 0x00 */ boost::function1<void, GadgetColliDetectArgs &> m_onContact;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ CharaColliBase *m_collider;
};

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for the collision handle (0x80103678; boost 1.32's definition). */
namespace boost {
template<> inline void shared_ptr<GadgetCharCollision>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The motion work of a model (class name from its weak_ptr instances; layout unknown). */
struct _stMtnPlayWork;

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The game state block in .bss (only the members used here; meanings unknown). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x98];
    /* 0x98 */ float field_0x98; /* an angle (radians) */
    /* 0x9C */ u8 field_0x9C[0xA8 - 0x9C];
    /* 0xA8 */ s32 field_0xA8; /* the stage number */
    /* 0xAC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Loads the model fileName, from the stage's files when stage is set (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int stage, const char *fileName);
/* Plays motion number motion of model variant index, and returns the motion work of variant index
 * (behaviour inferred). */
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
/* Sets the speed of a motion (behaviour inferred; arg1 unknown). */
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float speed);
/* Shows and hides the model (behaviour inferred). */
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);

/* The collider manager, a box collider of the given size for charInfo, and a cylinder collider
 * (behaviour inferred; the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4,
                                              int arg5, const Vec3f *offset,
                                              const Vec3f *rotation, int arg8);
boost::shared_ptr<CharaColliBase> fn_800C7A84(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, u16 arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height,
                                              float radius, int arg9);
extern const Vec3f lbl_805E07B8;
/* Wraps collider in a new handle; draws the handle's shape for debugging, advances it by dt and
 * sets two of its options (behaviour inferred; the arguments of the last are unknown). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
void fn_8015BDC4(GadgetCharCollision *handle, int arg1, int arg2);

/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args,
                 void (*callback)(boost::shared_ptr<CharInfo> hit, void *user), void *user);
/* Whether the character can be moved by gadgets, and whether it left the ground (behaviours and
 * meanings inferred). */
bool fn_800B7938(const boost::shared_ptr<CharInfo> &character);
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &character);

/* Starts effect id at position, facing direction; makes a running effect follow matrix
 * (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D7DC(const Vec3f *position,
                                                           const Vec3f &direction, int id);
void fn_80425E68(Effect::System::EffectElement *effect, const Mtx34f *matrix);
/* Plays sound id from source, at position when given (behaviour inferred; the other arguments are
 * unknown). */
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);

/* Vector and matrix helpers (behaviour inferred): vector -= value, vector * scale, and the inverse
 * of a matrix. */
void fn_8040CE04(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040BEF8(Mtx34f *result, const Mtx34f *matrix);
}

/* The kind CharInfo keeps at 0x50; 1 for a player (meaning inferred; CharInfo.h does not map it). */
inline s32 GetCharKind(const CharInfo *info) {
    return *(const s32 *)((const u8 *)info + 0x50);
}

/* A player's velocity (meaning inferred; Player.h does not map it). */
inline const Vec3f &GetPlayerVelocity(const Player::PlayerBase *player) {
    return *(const Vec3f *)((const u8 *)player + 0xE8);
}

/* result = a - b (name inferred), returned through the caller's temporary. */
inline const Vec3f &Subtract(Vec3f *result, const Vec3f *a,
                                     const Vec3f *b) {
    fn_8040CD74(result, a, b);
    return *result;
}

namespace Fan {

/* The parameters the owner supplies (names from the parameter table). */
struct FanParams {
    FanParams()
        : type(0), form(0), x(10.0f), y(10.0f), z(10.0f), power(10.0f), secActive(-1.0f),
          secNegative(-1.0f), disp(1), id(-1) {}

    /* 0x00 */ int type;  /* 0 upward, 1 sideways */
    /* 0x04 */ int form;  /* 0 cylinder, 1 box */
    /* 0x08 */ float x;   /* radius or width */
    /* 0x0C */ float y;   /* (unused) or height */
    /* 0x10 */ float z;   /* height or depth */
    /* 0x14 */ float power;
    /* 0x18 */ float secActive;
    /* 0x1C */ float secNegative;
    /* 0x20 */ int disp;  /* the model is shown */
    /* 0x24 */ int id;    /* the switch, or -1 */
};
SIZE_ASSERT(FanParams, 0x28);

/* A 32-bit flag word (the class name is unknown; its out-of-line members are the fn_80014E04
 * family above). */
struct FanFlags {
    FanFlags() {
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

/* What the collision callback passes to the hit functions (name inferred). */
struct FanHitContext {
    FanHitContext() : m_params(0), m_hit(false) {}

    /* 0x00 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x10 */ FanParams *m_params;
    /* 0x14 */ bool m_hit;
};
SIZE_ASSERT(FanHitContext, 0x18);

class Fan : public Gadget {
  public:
    Fan(Task *parent, UnkPlacement_800C9CD0 *owner, int stage);
    virtual ~Fan();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Instantiate a FAN and a FAN STAGE under task layer 11 (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);
    static void CreateStage(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void RebuildTransform();
    void OnColliDetect(ColliDetectArgs &args);
    void SetupCollision();
    void UpdateFan(float dt);
    /* Whether target stands within the fan's collider, seen from above. It takes the character
     * infos rather than their positions: the original reads each position after storing the
     * previous one, which loads through Vec3f references would let the scheduler hoist. */
    bool IsInside(CharInfo *target, CharInfo *fan);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x48 */ boost::weak_ptr<CharInfo> m_target; /* the character in the stream */
    /* 0x50 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x58 */ boost::weak_ptr<_stMtnPlayWork> m_motion;
    /* 0x60 */ FanParams m_params;
    /* 0x88 */ s32 m_state; /* 0 new, 1 running */
    /* 0x8C */ float m_timer;
    /* 0x90 */ float m_spin;  /* the change of m_speed per frame */
    /* 0x94 */ float m_speed; /* of the blade motion, 0 to 1 */
    /* 0x98 */ FanFlags m_flags; /* 0 holding a character, 2 on, 3 timed, 4 stage sound */
    /* 0x9C */ u32 field_0x9C;
};
SIZE_ASSERT(Fan, 0xA0);

/* The model file (name inferred). */
const char *s_fileName = "FAN.BIN";
const char *Fan::s_taskName = "Fan";

/* Pushes a character along the fan's axis, upward fans less the higher it is (name inferred). */
static void PushCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    FanHitContext *context = static_cast<FanHitContext *>(user);

    boost::shared_ptr<GadgetCharInfo> fan = context->m_chara.lock();
    if (fan.get() == 0) {
        return;
    }
    Vec3f direction(0.0f, 1.0f, 0.0f);
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, fan->m_rotation.y);
    fn_8040B884(&matrix, fan->m_rotation.x);
    fn_8040B80C(&matrix, fan->m_rotation.z);
    AssignVector(direction, fn_8040B67C(&matrix, &direction));
    fn_8040CE38(&direction, context->m_params->power);
    if (context->m_params->type == 0) {
        float height = hit->m_position.y - fan->m_position.y;
        if (height < 0.0f) {
            height = 0.0f;
        }
        float top = 2.0f * context->m_params->z + 5.0f * std::cos(lbl_8057D6A0.field_0x98);
        fn_8040CE38(&direction, (top - height) / top);
    }
    hit->PushCommand(new AccelerateCommand(direction, 4));
    context->m_hit = true;
    context->m_target = hit;
}

/* Stops a player and holds it in the stream (name inferred). */
static void CatchCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    FanHitContext *context = static_cast<FanHitContext *>(user);

    if (context != 0 && GetCharKind(hit.get()) == 1) {
        hit->PushCommand(new ClrVelocityCommand(4));
        hit->PushCommand(new Player::StreamCommand(true, 4));
        context->m_hit = true;
        context->m_target = hit;
    }
}

Fan::Fan(Task *parent, UnkPlacement_800C9CD0 *owner, int stage) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x10, 4);
    LoadFromOwner(owner);
    m_model = fn_8015F800(stage, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80160A84(m_model.get(), 0, 1);
    m_motion = fn_801602B4(m_model.get(), 0);
    SetupCollision();
    RebuildTransform();
    if (m_params.disp == 0) {
        fn_80162790(m_model.get());
    }
}

Fan::~Fan() {
    if (!m_effect.expired()) {
        if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
            effect->Retire();
        }
        m_effect.reset();
    }
    boost::shared_ptr<CharInfo> target = m_target.lock();
    if (target.get() != 0 && fn_800B7938(target)) {
        target->PushCommand(new Player::StreamCommand(false, 4));
    }
}

void Fan::Behave(float dt) {
    m_flags.Reset(0);
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
    UpdateFan(dt);

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1: {
        boost::shared_ptr<CharInfo> target = m_target.lock();
        if (target.get() != 0 && fn_800B7938(target)) {
            bool release = true;
            if (m_params.type == 0) {
                if (IsInside(target.get(), m_chara.get())) {
                    release = false;
                    if (!m_flags.Test(0)) {
                        Player::PlayerBase *player =
                            dynamic_cast<Player::PlayerBase *>(target.get());
                        if (player != 0) {
                            player->PushCommand(new AccelerateCommand(
                                fn_8040CDA8(&GetPlayerVelocity(player), -30.0f), 4));
                        }
                    }
                }
                if (!m_flags.Test(0) && fn_800B7AD0(target)) {
                    release = true;
                }
            } else if (m_params.type == 1) {
                if (m_flags.Test(0)) {
                    release = false;
                }
            }
            if (release) {
                target->PushCommand(new Player::StreamCommand(false, 4));
                m_target.reset();
            }
        }
        break;
    }
    case 2: /* never set; its empty case is in the original's jump code */
        break;
    }

    RebuildTransform();
}

void Fan::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int form = m_params.form;
    int disp = m_params.disp;

    LoadFromOwner(owner);
    RebuildTransform();
    if (form != m_params.form) {
        m_handle.reset();
        SetupCollision();
    }
    if (disp != m_params.disp) {
        if (m_params.disp != 0) {
            fn_8016271C(m_model.get());
        } else {
            fn_80162790(m_model.get());
        }
    }
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
    }
    if (m_handle.get() != 0) {
        m_handle->m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
        switch (m_params.form) {
        case 1:
            m_handle->m_collider->SetOffset(&Vec3f(0.0f, m_params.y, 0.0f), -2);
            break;
        case 0:
            m_handle->m_collider->SetOffset(&Vec3f(0.0f, m_params.z, 0.0f), -2);
            break;
        }
    }
}

void Fan::fn_800CBC1C() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void Fan::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_timer = 0.0f;
    m_spin = 0.0f;
    m_speed = 1.0f;
    m_flags.Clear();
    m_flags.Set(2);
    if (m_params.secActive > 0.0f && m_params.secNegative > 0.0f) {
        m_flags.Set(3);
        m_timer = m_params.secActive;
    }
    if (m_params.type == 0) {
        m_chara->m_rotation.x = 0.0f;
    } else {
        m_chara->m_rotation.x = 90.0f * (lbl_805FAD38 / 180.0f);
    }
    if (m_params.type == 1 && lbl_8057D6A0.field_0xA8 == 501) {
        m_flags.Set(4);
    }
}

void Fan::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        m_model->fn_801633DC(&matrix);
        if (!m_effect.expired()) {
            if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
                fn_80425E68(effect.get(), &matrix);
            }
        }
    }
}

void Fan::OnColliDetect(ColliDetectArgs &args) {
    if (m_flags.Test(2)) {
        boost::shared_ptr<CharInfo> nearest(
            fn_800A8A7C(fn_80074194(), &m_chara->m_position));
        if (nearest.get() != 0 && !fn_800B7938(nearest)) {
            FanHitContext context;

            context.m_chara = m_chara;
            context.m_params = &m_params;
            fn_8015AA84(args, CatchCharacter, &context);
            if (context.m_hit) {
                m_target = context.m_target;
            }
        }
    }
    if (m_flags.Test(2)) {
        FanHitContext context;

        context.m_chara = m_chara;
        context.m_params = &m_params;
        fn_8015AA84(args, PushCharacter, &context);
        if (context.m_hit) {
            m_flags.Set(0);
        }
    }
}

void Fan::SetupCollision() {
    Vec3f size;
    fn_800091EC(&size, &Vec3f(1.0f, 1.0f, 1.0f));
    boost::shared_ptr<CharaColliBase> collider;

    switch (m_params.form) {
    case 1:
        collider = fn_800C7C34(fn_80021B08(), m_chara, &size, 0x40100, 10, 0, &lbl_805E07A0,
                               &lbl_805E07B8, 0);
        break;
    case 0: {
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        rotation.x = 90.0f * (lbl_805FAD38 / 180.0f);
        rotation.y = 0.0f;
        rotation.z = 0.0f;
        collider = fn_800C7A84(fn_80021B08(), m_chara, 0x40100, 10, 0, &lbl_805E07A0,
                               &rotation, 1.0f, 1.0f, 0);
        break;
    }
    }
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(m_handle.get(), boost::bind(&Fan::OnColliDetect, this, _1));
    m_handle->m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
    switch (m_params.form) {
    case 1: {
        Vec3f offset(0.0f, m_params.y, 0.0f);
        m_handle->m_collider->SetOffset(&offset, -2);
        break;
    }
    case 0: {
        Vec3f offset(0.0f, m_params.z, 0.0f);
        m_handle->m_collider->SetOffset(&offset, -2);
        break;
    }
    }
    fn_8015BDC4(m_handle.get(), 1, 2);
}

void Fan::UpdateFan(float dt) {
    if (m_params.id >= 0) {
        UnkSwitchBank_800CB69C *manager = fn_800CB69C();
        if (fn_8016904C(manager, m_params.id) == 1) {
            m_flags.Set(2);
        } else {
            m_flags.Reset(2);
        }
    }

    if (m_flags.Test(2)) {
        if (m_effect.expired()) {
            m_effect = fn_8015D7DC(&m_chara->m_position, Vec3f(0.0f, 0.0f, -1.0f), 0xD9);
        }
        if (!m_motion.expired()) {
            m_spin += 0.01f;
            m_speed += m_spin;
            if (m_speed > 1.0f) {
                m_speed = 1.0f;
            }
            if (boost::shared_ptr<_stMtnPlayWork> work = m_motion.lock()) {
                fn_80416DC8(work.get(), 0, m_speed);
            }
            if (m_flags.Test(4)) {
                fn_801D53C4(fn_80053164(), 0x101A, 0, this, 2, 0, 2);
            } else {
                fn_801D53C4(fn_80053164(), 0x101A, &m_chara->m_position, this, 3, 0, 5);
            }
        }
    } else {
        if (!m_effect.expired()) {
            if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
                effect->Retire();
            }
            m_effect.reset();
        }
        if (!m_motion.expired()) {
            m_spin += 0.01f;
            m_speed -= m_spin;
            if (m_speed < 0.0f) {
                m_speed = 0.0f;
            }
            if (boost::shared_ptr<_stMtnPlayWork> work = m_motion.lock()) {
                fn_80416DC8(work.get(), 0, m_speed);
            }
        }
    }

    if (m_flags.Test(3)) {
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            if (m_flags.Test(2)) {
                m_flags.Reset(2);
                m_timer = m_params.secNegative;
                m_spin = 0.0f;
            } else {
                m_flags.Set(2);
                m_timer = m_params.secActive;
                m_spin = 0.0f;
            }
        }
    }
}

bool Fan::IsInside(CharInfo *target, CharInfo *fan) {
    bool inside = false;

    switch (m_params.form) {
    case 1: {
        Vec3f offset(target->m_position.x, 0.0f, target->m_position.z);
        Vec3f origin(fan->m_position.x, 0.0f, fan->m_position.z);
        Mtx34f matrix;
        Mtx34f inverse;

        fn_8040CE04(&offset, &origin);
        fn_8040B6F8(&matrix);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040BEF8(&inverse, &matrix);
        fn_8040C148(&matrix, &inverse);
        AssignVector(offset, fn_8040B67C(&matrix, &offset));
        float halfWidth = 7.5f + m_params.x;
        float halfDepth = 7.5f + m_params.z;
        if (-halfWidth <= offset.x && offset.x <= halfWidth && -halfDepth <= offset.z &&
            offset.z <= halfDepth) {
            inside = true;
        }
        break;
    }
    case 0: {
        Vec3f flatPoint(target->m_position.x, 0.0f, target->m_position.z);
        Vec3f flatCenter(fan->m_position.x, 0.0f, fan->m_position.z);

        if (fn_8040CF50(&Subtract(&Vec3f(), &flatPoint, &flatCenter)) <= 7.5f + m_params.x) {
            inside = true;
        }
        break;
    }
    }
    return inside;
}

} // namespace Fan

/* The labels of the "type", "form", "X", "Y", "Z" and "Id" parameters; X, Y and Z are labelled by
 * the form last shown. */
extern "C" const char *fn_80102328(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_typeLabels[2] = {"type : UpperWay", "type : SideWay"};
    static const char *s_formLabels[2] = {"form : Cylinder", "form : Box"};
    static const char *s_xLabels[2] = {"radius", "width"};
    static const char *s_yLabels[2] = {"Not in use", "height"};
    static const char *s_zLabels[2] = {"height", "depth"};
    static const char *s_idLabels[2] = {"Id -> Not Available", "Id -> Available"};
    static int s_form = 0;

    if (cursor->index == 0) {
        return s_typeLabels[cursor->value];
    } else if (cursor->index == 1) {
        int form = cursor->value;
        s_form = form;
        return s_formLabels[form];
    } else if (cursor->index == 2) {
        return s_xLabels[s_form];
    } else if (cursor->index == 3) {
        return s_yLabels[s_form];
    } else if (cursor->index == 4) {
        return s_zLabels[s_form];
    } else if (cursor->index == 9) {
        if (cursor->value >= 0) {
            return s_idLabels[1];
        }
        return s_idLabels[0];
    }
    return 0;
}

extern "C" void fn_80102300(void) {
    fn_8006B474("FAN.BIN", 1);
}

extern "C" void fn_801022FC(void) {}

void Fan::Fan::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Fan(parent, owner, 0);
}

extern "C" void fn_80102278(void) {
    fn_8006B474("FAN.BIN", 1);
}

extern "C" void fn_80102274(void) {}

void Fan::Fan::CreateStage(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Fan(parent, owner, 1);
}

static GadgetParamDesc lbl_80525C6C[] = {
    {1, "type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80102328, 0},
    {1, "form", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80102328, 0},
    {4, "X", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80102328, 0},
    {4, "Y", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80102328, 0},
    {4, "Z", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80102328, 0},
    {4, "power", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "sec_active", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "sec_negative", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "disp", 1, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Id", -1, -1, 0xFF, 0.0f, 0.0f, 0.0f, fn_80102328, 0},
    {0},
};

GadgetClassDesc lbl_80525E24 = {
    "FAN", fn_80102300, fn_801022FC, Fan::Fan::Instantiate, 0, 0, 0x1A, 1, 0, 10, lbl_80525C6C,
};

GadgetClassDesc lbl_80525E48 = {
    "FAN STAGE", fn_80102278, fn_80102274, Fan::Fan::CreateStage, 0, 0, 0x2594, 1, 0, 10,
    lbl_80525C6C,
};
